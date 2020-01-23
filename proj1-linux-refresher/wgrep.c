// File:	wgrep.c
// Author:	Michael Bruno
// Descr:	Command-line utility to search for string patterns
//          in a file and display lines with a found match to the terminal.

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>


int search_file(FILE *fp, char *pattern) {
// Searches each line of <filename> for the matching string <pattern>.
// If <pattern> is present in a line, output that line to the terminal.

	if (fp == NULL) {
    	printf("%s\n","wgrep: cannot open file");
    	exit(1);
	}

	size_t size = 0;
	char* linebuf = 0;

	while (getline(&linebuf, &size, fp) > 0) {
		if (strstr(linebuf,pattern)) {
			printf("%s", linebuf);
		}
	}

	free(linebuf);
	fclose(fp);

    return 0;
}


int main(int argc, char *argv[]) {

    if (argc == 1) {
    	printf("%s\n","wgrep: searchterm [file ...]");
    	exit(1);

    } else if (argc == 2) {
    	search_file(stdin, argv[1]);

    } else {
	    for (int i = 2; i < argc; i++) {
			search_file(fopen(argv[i], "r"), argv[1]);
		}
    }

	return 0;

}