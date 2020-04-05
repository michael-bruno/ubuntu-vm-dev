
build: test.prog.c ut_threads.c
	gcc -o ut_test test.prog.c ut_threads.c
	gcc -o ct_test context_test.c ut_threads.c

test:
	./ct_test
	./ut_test

clean:
	rm ut_test