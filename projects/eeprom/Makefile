obj-m := atc_eeprom.o


ARCH:=arm
CROSS_COMPILE:=arm-linux-
KDIR :=/root/Desktop/backup/linux-3.10.4

PWD := $(shell pwd)
default:
         
	 $(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
        
	  $(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean












