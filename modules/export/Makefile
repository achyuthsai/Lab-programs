obj-m :=summ.o
obj-m +=modeapp1.o
obj-m +=modeapp2.o
obj-m +=pcb.o
obj-m +=rtc.o
obj-m +=functions.o
obj-m +=myrtc.o

KDIR=/lib/modules/$(shell uname -r)/build

module:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean


