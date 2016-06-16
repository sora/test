KERNELSRCDIR := /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(shell pwd)
VERBOSE = 0

obj-m := tlptap.o

all:
	gcc -Wall -O -o wr wr.c
	gcc -Wall -O -o rd rd.c
	make -C $(KERNELSRCDIR) M=$(BUILD_DIR) V=$(VERBOSE) modules

clean:
	rm wr rd
	make -C $(KERNELSRCDIR) M=$(BUILD_DIR) clean

