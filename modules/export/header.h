#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>


#define SEC 		0x00
#define MIN 		0x02
#define HOUR		0x04
#define DAY 		0x06
#define DATE 		0x07
#define MONTH 		0x08
#define YEAR 		0x09
#define CMD_REG 	0x70
#define DATA_REG	0x71
#define STATUS_REG 	0x71


#define READ_SEC(sec)	{\
			outb(SEC,CMD_REG);\
			sec = inb(STATUS_REG);\
			}

			
#define READ_MIN(min)	{\
			outb(MIN,CMD_REG);\
			min = inb(STATUS_REG);\
			}

#define READ_HOUR(hou)	{\
			outb(HOUR,CMD_REG);\
			hou = inb(STATUS_REG);\
			}

#define READ_DAY(day)	{\
			outb(DAY,CMD_REG);\
			day = inb(STATUS_REG);\
			}


#define READ_DATE(dat)	{\
			outb(DATE,CMD_REG);\
			dat = inb(STATUS_REG);\
			}


#define READ_MONTH(mon)	{\
			outb(MONTH,CMD_REG);\
			mon = inb(STATUS_REG);\
			}


#define READ_YEAR(yea)	{\
			outb(YEAR,CMD_REG);\
			yea = inb(STATUS_REG);\
			}

extern int  mycalender(void);
extern int mytime(void);

