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
    	printf("wcat: cannot open file\n");
    	exit(1);
	}

	char c = fgetc(fp); 
    while (c != EOF) 
    { 
        printf("%c", c); 
        c = fgetc(fp); 
    } 
  
    fclose(fp);

    printf("\n");

    return 0;
}


int main(int argc, char *argv[]) {
	
	if (argc == 1) exit(0);

	for (int i = 1; i < argc; i++) {
		display_file(argv[i]);
	}

    return 0;
}