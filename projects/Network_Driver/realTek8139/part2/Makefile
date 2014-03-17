
#obj-m += napi_23.o
obj-m += napi_31.o

KDIR= /lib/modules/$(shell uname -r)/build
all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	rm -rf $(wildcard *.o *.ko mo* Mo* *.mod.c *.order )
