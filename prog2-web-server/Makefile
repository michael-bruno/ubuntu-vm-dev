webserver: eznet.o
		gcc -fsanitize=address -std=c99 -g -fno-omit-frame-pointer webserver.c -lbsd -owebserver eznet.o
eznet.o:
		gcc -c -g -fsanitize=address -Wall -Werror -DSHOW_LOG_ERROR eznet.c
clean:
		rm eznet.o