// File:    reveresemm.c
// Author:  Michael Bruno 
// Descr:   Program using mmap to reverse file contents.

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
        fprintf(stderr, "[reversemm] Usage: reversemm <filename>\n");
        exit(1);
    }

    int fd;
    char *fname = argv[1];

    fd = open (fname, O_RDWR);
    if(fd == -1){
        fprintf(stderr, "[reversemm] Could not open file %s: %s\n", fname, strerror(errno));
        exit(1);  
    }

    int len = fsize(argv[1]);

    char *p = mmap (0, len, PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        printf("[reversemm] Could not map file %s: %s\n", fname, strerror(errno));
        exit(1); 
    }

    for (int low=0, high=len-1; low < high; low++, high--) {
        char temp = p[low];
        p[low] = p[high];
        p[high] = temp;
    }

    munmap(p, len);
    close (fd);
  
}