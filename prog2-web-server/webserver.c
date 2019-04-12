// File: webserver.c
// Author: Michael Bruno
// Date: 3/29/2019

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

#include <stdbool.h>
#include <bsd/string.h>
#include <pthread.h>

#include "utils.h"

#define MAX_HEADERS 10

//const int BUFFSIZE = 256;

enum BUFFSIZE
{
    STR_SIZE = 100,
    PATH_SIZE = 256,
    HEADER_NAME = 50,
    HEADER_VALUE = 50,
    ERRSTR_SIZE = 100
};

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

// Signal handler
void handler(int sig) {
    if (sig == SIGPIPE) printf("\t[SIG] Connection broken: (%i)\n",sig);
}

// GLOBAL: settings structure instance
struct settings {
    const char *bindhost;   // Hostname/IP address to bind/listen on
    const char *bindport;   // Portnumber (as a string) to bind/listen on
    const char *root_directory;
    int max_processes;
} g_settings = {
    .bindhost = "localhost",    // Default: listen only on localhost interface
    .bindport = "5000",         // Default: listen on TCP port 5000
    .root_directory = ".",
    .max_processes = 5
};

// Parse commandline options and sets g_settings accordingly.
// Returns 0 on success, -1 on false...
int parse_options(int argc, char * const argv[]) {
    int ret = -1; 

    int op;
    while ((op = getopt(argc, argv, "h:p:r:w:")) > -1) {
        switch (op) {
            case 'h':
                g_settings.bindhost = optarg;
                break;
            case 'p':
                g_settings.bindport = optarg;
                break;
            case 'r':
                g_settings.root_directory = optarg;
                break;
            case 'w':
                if (!atoi(optarg)) {
                    printf("'%s' not a valid number of processes.\n",optarg);
                    goto cleanup;
                }
                g_settings.max_processes = atoi(optarg);
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
        req->verb = malloc(STR_SIZE);
        if (!req->verb) { rc = -3; goto cleanup; }

        int len = strlcpy(req->verb,token,STR_SIZE);
        if (len >= STR_SIZE) rc = -2;

    }
    else { rc = -4; goto cleanup; }

//  Get request PATH
    token = strtok_r(saveptr," ",&saveptr);
    if (!token) { rc = -5; goto cleanup; }

    // check if '/' in path
    if (strcspn(token,"/") == strlen(token) && strcspn(token,"\\") == strlen(token)) { rc = -5; goto cleanup; }

    req->path = malloc(PATH_SIZE);
    if (!req->path) { rc = -3; goto cleanup; }

    int len = strlcpy(req->path,token,PATH_SIZE);
    if (len >= PATH_SIZE) rc = -2;

//  Get request VERSION
    token = strtok_r(saveptr," ",&saveptr);
    if (!token) { rc = -6; goto cleanup; }

    // remove CRLF
    token[strcspn(token, "\r\n")] = 0;
    
    if (!strcmp(token,"HTTP/1.0") || !strcmp(token,"HTTP/1.1"))
    {
        req->version = malloc(STR_SIZE);
        if (!req->version) { rc = -3; goto cleanup; }

        int len = strlcpy(req->version,token,STR_SIZE);
        if (len >= STR_SIZE) rc = -2;
    }

//  Get request HEADERS
    int inc = 0, line_size;
    while((line_size = getline(&line, &size, in)) > 0)
    {
        if (!strcmp(line,"\r\n") || !strcmp(line,"\n")) break;

        char *lineptr = line;

        if (strlen(line) == 0) break;

        char *name = strtok_r(lineptr,":",&lineptr);
        if (!name) { rc = -7; goto cleanup; }

        char *value = strtok_r(++lineptr,"\r\n",&lineptr);
        if (!value) { rc = -7; goto cleanup; }

        http_header_t *new_header = malloc(sizeof(http_header_t));

        new_header->name = malloc(HEADER_NAME);
        new_header->value = malloc(HEADER_VALUE);

         if (!new_header->name || !new_header->value) { rc = -3; goto cleanup; }
        
        int len = (new_header->name,name,HEADER_NAME);
        if (len >= HEADER_NAME) rc = -2;

        len = strlcpy(new_header->value,value,HEADER_VALUE);
        if (len >= HEADER_VALUE) rc = -2;

        if (inc < MAX_HEADERS) req->headers[inc] = *new_header;

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

// Get file's extension.
char *get_filename_ext(char *filename) {
    char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "";
    return dot + 1;
}

// Parses a filepath and determines proper content type.
char *parse_content_type(char *path)
{
    char *ext = get_filename_ext(path);
    //printf("EXT: {%s}\n",ext);

    char *type = "application/octet-stream";

    if (!strcmp(ext,"jpeg") || !strcmp(ext,"jpg")) type = "image/jpeg";
    else if (!strcmp(ext,"html") || !strcmp(ext,"htm")) type = "text/html";
    else if (!strcmp(ext,"gif")) type =  "image/gif";
    else if (!strcmp(ext,"png")) type = "image/png";
    else if (!strcmp(ext,"css")) type = "text/css";
    else if (!strcmp(ext,"txt")) type = "text/plain";
    return type;

}

// Mutex
pthread_mutex_t num_lock;

int NUM_CLIENTS = 0;

// Connection handling logic: reads/echos lines of text until error/EOF,
// then tears down connection.
void *handle_client(void *args) {

    struct client_info *client = args;

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
 
    char *line = NULL;
    size_t len = 0u;
    ssize_t recd;

    http_request_t *request = NULL;
    int result = parseHttp(stream,&request);

    // initialize response
    http_response_t response = {
        .version = "HTTP/1.1",
        .status = "400 Bad Request",
        .num_headers = 0
    };

    http_header_t content_type = {
        .name = "Content-Type",
        .value = "text/plain"
    };

    char *ERROR_MSG = malloc(ERRSTR_SIZE);
    if (!ERROR_MSG) { strcpy(ERROR_MSG, "Malloc failure.\n"); }

    size_t size = 32u;
    FILE *fd = NULL;

    switch (result) {
    case 1:
        response.status = "200 OK";

        //printf("REQUEST: {%s %s %s}\n",request->verb,request->path,request->version);

        char *path = malloc(PATH_SIZE);
        if (!path) { strlcpy(ERROR_MSG, "Malloc failure.\n",ERRSTR_SIZE); }

        // current directory
        int len = snprintf(path,PATH_SIZE,"%s%s",g_settings.root_directory,request->path);
        if (len >= PATH_SIZE) response.status = "400 Bad Request";

        //printf("PATH: {%s}\n",path);

        fd = fopen(path, "r");

        if (!fd)
        {
            content_type.value = "text/plain";
            response.status = "404 Not Found";
            strcpy(ERROR_MSG, "File not found.\n");
        }
        else { content_type.value = parse_content_type(path); }
        free(path);

        break;
    case -1:
        strlcpy(ERROR_MSG, "Illegal HTTP stream\n",ERRSTR_SIZE);
        break;
    case -2:
        strlcpy(ERROR_MSG, "I/O error while reading request.\n",ERRSTR_SIZE);
        break;
    case -3:
        strlcpy(ERROR_MSG, "Malloc failure.\n",ERRSTR_SIZE);
        break;
    case -4:
        strlcpy(ERROR_MSG, "Illegal HTTP stream (Invalid verb)\n",ERRSTR_SIZE);
        break;
    case -5:
        strlcpy(ERROR_MSG, "Illegal HTTP stream (Invalid path)\n",ERRSTR_SIZE);
        break;
    case -6:
        strlcpy(ERROR_MSG, "Illegal HTTP stream (Invalid version)\n",ERRSTR_SIZE);
        break;
    case -7:
        strlcpy(ERROR_MSG, "Illegal HTTP stream (Invalid header)\n",ERRSTR_SIZE);
        break;
    default:
        strlcpy(ERROR_MSG, "Unexpected error..\n",ERRSTR_SIZE);
    }
    
    response.headers[response.num_headers] = content_type; response.num_headers++;

    // printf("VERSION: {%s}\n",response.version);
    // printf("STATUS: {%s}\n",response.status);

    // put to stream
    int resp;
    resp = fprintf(stream,"%s %s\n",response.version,response.status);

    for (int i = 0; i < response.num_headers; ++i) {
        resp = fprintf(stream,"%s: %s\n", response.headers[i].name, response.headers[i].value);
    }
    fflush(stream);

    // puts("HEADERS");

    char *fline = malloc(STR_SIZE);

    fprintf(stream,"\r\n");
    if (fd) {
        int read;
        //while(read = getline(&fline, &size, fd) >= 0)
        while(read = fread(fline,sizeof(char),STR_SIZE,fd))
        {
            //fprintf(stream,"%s",fline);
            fwrite(fline,sizeof(char),STR_SIZE,stream);
        }
        fclose(fd);
    }
    else { fputs(ERROR_MSG,stream); }
    free(fline);

cleanup:
    // puts("CLEANUP");

    // Shutdown this client
    for (int i = 0; i < request->num_headers; ++i) {
        free(request->headers[i].name);
        free(request->headers[i].value);
    }

    free(ERROR_MSG);

    free(request->verb);
    free(request->path);    
    free(request->version);
    free(request);

    if (stream) fclose(stream);
    free(line);

    destroy_client_info(args);

    pthread_mutex_lock(&num_lock);
    NUM_CLIENTS--;
    pthread_mutex_unlock(&num_lock);

    printf("\tSession ended.\n");
}


int main(int argc, char **argv) {
    int ret = 1;

    signal(SIGINT, handler);
    signal(SIGTERM, handler);
    signal(SIGPIPE, handler);

    // Initialize thread pool
    pthread_t THREADS[g_settings.max_processes];
    for (int i = 0; i < g_settings.max_processes; i++)
    {
        pthread_t new_thread; THREADS[i] = new_thread;
    }

    pthread_mutex_init(&num_lock, NULL);

    // Network server/client context
    int server_sock = -1;

    // Handle our options
    if (parse_options(argc, argv)) {
        printf("usage: %s [-p PORT] [-h HOSTNAME/IP] [-r ROOT_DIRECTORY] [-w MAX_PROCESSES]\n", argv[0]);
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

        //printf("\nTHREADS: {%i}\n",NUM_CLIENTS);

        // Wait for a connection on that socket
        if (wait_for_client(server_sock, &client)) {
            // Check to make sure our "failure" wasn't due to
            // a signal interrupting our accept(2) call; if
            // it was  "real" error, report it, but keep serving.
            if (errno != EINTR) { perror("unable to accept connection"); }
        } else if (NUM_CLIENTS < g_settings.max_processes) {
            blog("connection from %s:%d", client.ip, client.port);
            // handle_client(&client); // Client gets cleaned up in here
            pthread_create(&THREADS[NUM_CLIENTS],NULL,handle_client,(void *)&client);
            //destroy_client_info(&client);
            NUM_CLIENTS++;
        } else {
            blog("connection refused (THREAD LIMIT REACHED)");
        }
    }
    ret = 0;

cleanup:
    if (server_sock >= 0) close(server_sock);
    return ret;
}
