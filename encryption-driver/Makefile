obj-m += encrypt.o

KVERSION = $(shell uname -r)

all: modules rot47

modules:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) modules

rot47: rot47.c
	gcc rot47.c -o rot47

clean:
	make -C /lib/modules/$(KVERSION)/build M=$(PWD) clean
	rm rot47