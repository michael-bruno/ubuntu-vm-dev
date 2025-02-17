// File:	wzip.c
// Author:	Michael Bruno
// Descr:	Command-line utility to compress the contents of a file using 
//			the run-length encoding scheme (RLE) and output the compressed file to STDOUT.

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

#define MFILE ".temp"


int concat_file(char *filename, FILE *mfile) {
// Opens <filename> and concatenates contents to <mfile>.


	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {
    	printf("%s\n","wzip: cannot open file");
    	exit(1);
	}

	char c;
	while ((c = fgetc(fp)) != EOF) {
    	fputc(c, mfile);
	}

	return 0;
}


int zip_file(char *filename) {
// Opens <filename> and outputs compressed
// format to STDOUT.

	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {
    	printf("%s\n","wzip: cannot open file");
    	exit(1);
	}

	char c, last_c;
	int inc, counter;

	inc = counter = 0;
    while ((c = fgetc(fp)) != EOF) {

    	if (inc > 0 && last_c != c) {
    		fwrite(&counter,sizeof(int),1, stdout);
    		fputc(last_c, stdout);

    		counter = 0;
		}

    	inc++; counter++;
    	last_c = c;
    }

	fwrite(&counter,sizeof(int),1, stdout);
	fputc(last_c, stdout);

    fclose(fp);

    return 0;
}


int main(int argc, char *argv[]) {
	
	if (argc == 1) {
		printf("%s\n","wzip: file1 [file2 ...]");
		exit(1);
	}

	FILE *mfile = fopen(MFILE, "w");

	for (int i = 1; i < argc; i++) {
		concat_file(argv[i],mfile);
	}

	fclose(mfile);
	zip_file(MFILE);

	remove(MFILE);

    return 0;
}