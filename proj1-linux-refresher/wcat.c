// File:	wcat.c
// Author:	Michael Bruno
// Descr:	Command-line utility to display the contents of
//			the given files.

#include <stdio.h> 
#include <stdlib.h>


int display_file(char *filename) {
// Opens <filename> and outputs file
// contents to the console.

	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {
    	printf("%s\n","wcat: cannot open file");
    	exit(1);
	}

	char c; 
    while ((c = fgetc(fp)) != EOF) {
    	printf("%c", c);
    }

    fclose(fp);

    return 0;
}


int main(int argc, char *argv[]) {
	
	if (argc == 1) exit(0);

	for (int i = 1; i < argc; i++) {
		display_file(argv[i]);
	}

    return 0;
}