webserver: eznet.o utils.o webserver.c
		gcc -fsanitize=address -g -fno-omit-frame-pointer webserver.c -lbsd -owebserver eznet.o utils.o
eznet.o:
		gcc -c -g -fsanitize=address -Wall -Werror -DSHOW_LOG_ERROR eznet.c
utils.o:
		gcc -c -g -fsanitize=address -Wall -Werror -DSHOW_LOG_ERROR utils.c
clean:
		rm eznet.o