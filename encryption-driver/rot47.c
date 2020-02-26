//	File:	rot47.c
//	Author:	Michael Bruno
//	Descr:	Client program to utilize ROT-47 device driver.

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define max 40
#define device "/dev/encrypt"

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("rot47: no string supplied.\n");
		exit(1);
	}
	
	int fd = open(device, O_RDWR);
	if (fd == -1) {
        perror("Unable to open device");
        exit(1);
    }

    for (int i=1; i < argc; i++) {

	    char buff[max], eread[max];
	    strncpy(buff, argv[i], strlen(argv[i]));

		int result = write(fd, buff, strlen(buff));
		read(fd, eread, strlen(buff));
		puts(eread);

		memset(buff,0,max);
		memset(eread,0,max);
	}

	close(fd);

	return 0;
}