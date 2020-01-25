// File:	wunzip.c
// Author:	Michael Bruno
// Descr:	Command-line utility to decompress the contents of a file using 
//			the run-length encoding scheme (RLE) and output the result to stdin.

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


int unzip_file(char *filename) {
// Opens <filename> and outputs decompressed
// format to STDOUT.

	FILE *fp = fopen(filename, "r");

	if (fp == NULL) {
    	printf("%s\n","wunzip: cannot open file");
    	exit(1);
	}

	int count; char character;
	while (fread(&count, sizeof(int),1,fp) > 0) {

		fread(&character, sizeof(char),1,fp);
		for (int i = 0; i < count; ++i) {
			fputc(character, stdout);
		}
	}

    fclose(fp);

    return 0;
}


int main(int argc, char *argv[]) {
	
	if (argc == 1) {
		printf("%s\n","wunzip: file1 [file2 ...]");
		exit(1);
	}

	for (int i = 1; i < argc; i++) {
		unzip_file(argv[i]);
	}

    return 0;
}