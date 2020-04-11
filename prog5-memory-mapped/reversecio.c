// File:    reversecio.c
// Author:  Michael Bruno 
// Descr:   Program using c library calls to reverse file contents.

#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int fsize(const char *filename) {
    struct stat info;
    stat(filename, &info);

    return info.st_size;
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        fprintf(stderr, "[reversecio] Usage: reversemm <filename>\n");
        exit(1);
    }

    FILE *fd;
    char *fname = argv[1];

    if ((fd = fopen(fname, "r+")) == NULL) {
        fprintf(stderr, "[reversecio] Could not open file %s: %s\n", fname, strerror(errno));
        exit(1);  
    }

    int len = fsize(argv[1]);

    char *buffer;
    if ((buffer = malloc(len)) == NULL) {
        printf("[reversecio] Could not malloc %s: %s\n", fname, strerror(errno));
        exit(1); 
    }

    fread(buffer, len,1,fd);

    for (int low=0, high=len-1; low < high; low++, high--) {
        char temp = buffer[low];
        buffer[low] = buffer[high];
        buffer[high] = temp;
    }

    fseek(fd, 0, SEEK_SET);
    fwrite(buffer, len,1,fd);

    free(buffer);
    fclose(fd);
  
}