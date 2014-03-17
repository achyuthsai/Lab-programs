obj-m :=led.o

KDIR=/home/morning/achyuth/embedded/rootFS/lib/modules/3.0.4/build


all:
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:  
	$(MAKE) ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) SUBDIRS=$(PWD) clean

