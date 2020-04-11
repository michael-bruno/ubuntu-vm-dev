reversemm: reversemm.c reversecio.c
	gcc -o reversemm reversemm.c
	gcc -o reversecio reversecio.c

test-mm:
	hexdump med_test | head -n 5
	./reversemm med_test
	hexdump med_test | head -n 5
	./reversemm med_test
	hexdump med_test | head -n 5

test-cio:
	hexdump med_test | head -n 5
	./reversecio med_test
	hexdump med_test | head -n 5
	./reversecio med_test
	hexdump med_test | head -n 5

test-time:
	time -v ./reversemm med_test
	time -v ./reversecio med_test

test:
	make test-mm
	make test-cio

clean:
	rm reversemm
	rm reversecio