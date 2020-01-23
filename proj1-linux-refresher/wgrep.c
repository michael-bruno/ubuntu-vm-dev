// File:	wgrep.c
// Author:	Michael Bruno
// Descr:	Command-line utility to display the search for string patterns
//          in a file.

#include <stdio.h> 
#include <stdlib.h>


int main(int argc, char*argv[]) {

    if (argc == 1) exit(0);

    for (int i = 1; i < argc; i++) {
		display_file(argv[i]);
	}

	return 0;

}