obj-m :=rtc_pro.o

KDIRS=/lib/modules/$(shell uname -r)/build


all:
	$(MAKE) -C $(KDIRS) SUBDIRS=$(PWD) modules

clean: 
	$(MAKE) -C $(KDIRS) SUBDIRS=$(PWD) clean


