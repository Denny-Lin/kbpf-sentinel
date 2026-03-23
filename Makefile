# Makefile
KDIR ?= /root/kernel-build/kernel-src
PWD := $(shell pwd)

obj-m += kmod/sentinel_main.o

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean