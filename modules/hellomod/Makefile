obj-m:=hello.o 

KDIR=/lib/modules/$(shell uname -r)/build
#KDIR=/lib/modules/$(shell uname -r))/build

all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
