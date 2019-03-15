/* Echo Server: an example usage of EzNet
 * (c) 2016, Bob Jones University
 */

#define _GNU_SOURCE

#include <stdbool.h>    // For access to C99 "bool" type
#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // Other standard library functions
#include <string.h>     // Standard string functions
#include <errno.h>      // Global errno variable

#include <stdarg.h>     // Variadic argument lists (for blog function)
#include <time.h>       // Time/date formatting function (for blog function)

#include <unistd.h>     // Standard system calls
#include <signal.h>     // Signal handling system calls (sigaction(2))

#include "eznet.h"      // Custom networking library

#include <bsd/string.h>

#define MAX_HEADERS 10

const int BUFFSIZE = 256;

typedef struct http_header {
    char *name;
    char *value;
} http_header_t;

typedef struct http_request {
    char *verb;
    char *path;
    char *version;
    int num_headers;
    http_header_t headers[MAX_HEADERS];
} http_request_t;

typedef struct http_response {
    char *version;
    char *status;
    int num_headers;
    http_header_t headers[MAX_HEADERS];
} http_response_t;

// Generic log-to-stdout logging routine
// Message format: "timestamp:pid:user-defined-message"
void blog(const char *fmt, ...) {
    // Convert user format string and variadic args into a fixed string buffer
    char user_msg_buff[256];
    va_list vargs;
    va_start(vargs, fmt);
    vsnprintf(user_msg_buff, sizeof(user_msg_buff), fmt, vargs);
    va_end(vargs);

    // Get the current time as a string
    time_t t = time(NULL);
    struct tm *tp = localtime(&t);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tp);

    // Print said string to STDOUT prefixed by our timestamp and pid indicators
    printf("%s:%d:%s\n", timestamp, getpid(), user_msg_buff);
}

// GLOBAL: settings structure instance
struct settings {
    const char *bindhost;   // Hostname/IP address to bind/listen on
    const char *bindport;   // Portnumber (as a string) to bind/listen on
} g_settings = {
    .bindhost = "localhost",    // Default: listen only on localhost interface
    .bindport = "5000",         // Default: listen on TCP port 5000
};

// Parse commandline options and sets g_settings accordingly.
// Returns 0 on success, -1 on false...
int parse_options(int argc, char * const argv[]) {
    int ret = -1; 

    char op;
    while ((op = getopt(argc, argv, "h:p:")) > -1) {
        switch (op) {
            case 'h':
                g_settings.bindhost = optarg;
                break;
            case 'p':
                g_settings.bindport = optarg;
                break;
            default:
                // Unexpected argument--abort parsing
                goto cleanup;
        }
    }

    ret = 0;
cleanup:
    return ret;
}

// Returns 1 on success,
// -1 on Illegal HTTP stream
// -2 on I/O error while reading request.
// -3 on malloc failure
// -4 on Illegal HTTP stream (Invalid verb)
// -5 on Illegal HTTP stream (Invalid path)
// -6 on Illegal HTTP stream (Invalid version)
// -7 on Illegal HTTP stream (Invalid header)

int parseHttp(FILE *in, http_request_t **request) 
{
    size_t size = 32u;
    int rc = -1;

    http_request_t *req = NULL;

    req = malloc(sizeof(http_request_t));
    if (!req) { rc = -3; goto cleanup; }

    req->verb = 0;
    req->path = 0;
    req->version = 0;
    req->num_headers = 0;

    char *line = NULL;

    size_t read1 = getline(&line, &size, in);
    char *saveptr = line;

//  Get request VERB
    char *token = strtok_r(saveptr," ",&saveptr);
    if (!token) { rc = -4; goto cleanup; }

    if (!strcmp(token,"GET") || !strcmp(token,"POST"))
    {
        req->verb = malloc(BUFFSIZE);
        strlcpy(req->verb,token,BUFFSIZE);
    }
    else { rc = -4; goto cleanup; }

//  Get request PATH
    token = strtok_r(saveptr," ",&saveptr);
    if (!token) { rc = -5; goto cleanup; }

    // check if '/' in path
    if (strcspn(token,"/") == strlen(token)) { rc = -5; goto cleanup; }
    req->path = malloc(BUFFSIZE);
    strlcpy(req->path,token,BUFFSIZE);

//  Get request VERSION
    token = strtok_r(saveptr," ",&saveptr);
    if (!token) { rc = -6; goto cleanup; }

    // remove CRLF
    token[strcspn(token, "\r\n")] = 0;
    
    if (!strcmp(token,"HTTP/1.0") || !strcmp(token,"HTTP/1.1"))
    {
        req->version = malloc(BUFFSIZE);
        strlcpy(req->version,token,BUFFSIZE);
    }

//  Get request HEADERS
    int inc = 0;
    while(getline(&line, &size, in) >= 0)
    {
        if (!strcmp(line,"\r\n")) break;

        char *lineptr = line;

        if (strlen(line) == 0) break;

        char *name = strtok_r(lineptr,":",&lineptr);
        if (!name) { rc = -7; goto cleanup; }

        char *value = strtok_r(++lineptr,"\r\n",&lineptr);
        if (!value) { rc = -7; goto cleanup; }

        http_header_t *new_header = malloc(sizeof(http_header_t));

        new_header->name = malloc(BUFFSIZE);
        new_header->value = malloc(BUFFSIZE);

        strlcpy(new_header->name,name,BUFFSIZE);
        strlcpy(new_header->value,value,BUFFSIZE);

        if (inc <= MAX_HEADERS) req->headers[inc] = *new_header;

        free(new_header);
        inc++;
    }

    req->num_headers = inc;

    rc = 1;
cleanup:
    free(line);
    *request = req;
    return rc;
}


// GLOBAL: flag indicating when to shut down server
volatile bool server_running = false;

// SIGINT handler that detects Ctrl-C and sets the "stop serving" flag
void sigint_handler(int signum) {
    blog("Ctrl-C (SIGINT) detected; shutting down...");
    server_running = false;
}

// Connection handling logic: reads/echos lines of text until error/EOF,
// then tears down connection.
void handle_client(struct client_info *client) {
    FILE *stream = NULL;

    // Wrap the socket file descriptor in a read/write FILE stream
    // so we can use tasty stdio functions like getline(3)
    // [dup(2) the file descriptor so that we don't double-close;
    // fclose(3) will close the underlying file descriptor,
    // and so will destroy_client()]
    if ((stream = fdopen(dup(client->fd), "r+"))== NULL) {
        perror("unable to wrap socket");
        goto cleanup;
    }
 
    // Echo all lines
    char *line = NULL;
    size_t len = 0u;
    ssize_t recd;

    http_request_t *request = NULL;
    int result = 0;

    result = parseHttp(stream,&request);

    // initialize response
    http_response_t response = {
        .version = "HTTP/1.1",
        .status = "400 Bad Request",
        .num_headers = 0
    };

    switch (result) {
    case 1:
        response.status = "200 OK";

        printf("Verb: %s\n", request->verb);
        printf("Path: %s\n", request->path);
        printf("Version: %s\n", request->version);
        printf("\n%d header(s):\n", request->num_headers);
        for (int i = 0; i < request->num_headers; ++i) {
            printf("* %s is %s\n", request->headers[i].name, request->headers[i].value);
        }
        break;
    case -1:
        fprintf(stderr, "** ERROR: Illegal HTTP stream.\n");
        break;
    case -2:
        fprintf(stderr, "** ERROR: I/O error while reading request.\n");
        break;
    case -3:
        fprintf(stderr, "** ERROR: malloc failure.\n");
        break;
    case -4:
        fprintf(stderr, "** ERROR: Illegal HTTP stream (Invalid verb)\n");
        break;
    case -5:
        fprintf(stderr, "*** ERROR: Illegal HTTP stream (Invalid path)\n");
        break;
    case -6:
        fprintf(stderr, "** ERROR: Illegal HTTP stream (Invalid version)\n");
        break;
    case -7:
        fprintf(stderr, "** ERROR: Illegal HTTP stream (Invalid header)\n");
        break;
    default:
        printf("Unexpected return code %d.\n", result);
    }

    // DO STUFF
    size_t size = 32u;
    FILE *fd = NULL;
    char *fline = NULL;

    if (result == 1)
    {
        char *path = malloc(BUFFSIZE);

        // current directory
        snprintf(path,BUFFSIZE,".%s",request->path);
        //printf("PATH: {%s}\n",path);
        fd = fopen(path, "r");
        if (!fd) response.status = "404 Not Found";

        free(path);
    }

    // put to stream
    fprintf(stream,"%s %s\n",response.version,response.status);
    for (int i = 0; i < response.num_headers; ++i) {
        fprintf(stream,"%s: %s\n", response.headers[i].name, response.headers[i].value);
    }

    fprintf(stream,"\r\n");
    if (fd) {
        int read;
        while(read = getline(&fline, &size, fd) >= 0)
        {
            fprintf(stream,"%s",fline);
        }
        fclose(fd);
    }

cleanup:
    // Shutdown this client
    for (int i = 0; i < request->num_headers; ++i) {
        free(request->headers[i].name);
        free(request->headers[i].value);
    }

    free(request->verb);
    free(request->path);    
    free(request->version);
    free(request);

    if (stream) fclose(stream);
    destroy_client_info(client);
    free(line);
    printf("\tSession ended.\n");
}

int main(int argc, char **argv) {
    int ret = 1;

    // Network server/client context
    int server_sock = -1;

    // Handle our options
    if (parse_options(argc, argv)) {
        printf("usage: %s [-p PORT] [-h HOSTNAME/IP]\n", argv[0]);
        goto cleanup;
    }

    // Install signal handler for SIGINT
    struct sigaction sa_int = {
        .sa_handler = sigint_handler
    };
    if (sigaction(SIGINT, &sa_int, NULL)) {
        LOG_ERROR("sigaction(SIGINT, ...) -> '%s'", strerror(errno));
        goto cleanup;
    }

    // Start listening on a given port number
    server_sock = create_tcp_server(g_settings.bindhost, g_settings.bindport);
    if (server_sock < 0) {
        perror("unable to create socket");
        goto cleanup;
    }
    blog("Bound and listening on %s:%s", g_settings.bindhost, g_settings.bindport);

    server_running = true;
    while (server_running) {
        struct client_info client;

        // Wait for a connection on that socket
        if (wait_for_client(server_sock, &client)) {
            // Check to make sure our "failure" wasn't due to
            // a signal interrupting our accept(2) call; if
            // it was  "real" error, report it, but keep serving.
            if (errno != EINTR) { perror("unable to accept connection"); }
        } else {
            blog("connection from %s:%d", client.ip, client.port);
            handle_client(&client); // Client gets cleaned up in here
        }
    }
    ret = 0;

cleanup:
    if (server_sock >= 0) close(server_sock);
    return ret;
}

