#ifndef MY_RTC_IOCTL
#define MY_RTC_IOCTL

#include<asm/ioctl.h>

#define		SEC	0x00
#define		MIN	0x02
#define		HOU	0x04
#define		DAY	0x06
#define		DATE	0x07
#define		MON	0x08
#define		YEA	0x09

#define		CMD	0x70
#define		STATUS	0x71
#define		DATA	0x71


#define		READ_SEC(a)	{ outb(SEC, CMD); \
				  a = inb(DATA); \
				}
#define		READ_MIN(a)	{ outb(MIN, CMD); \
				  a = inb(DATA); \
				}
#define		READ_HOU(a)	{ outb(HOU, CMD); \
				  a = inb(DATA); \
				}
#define		READ_DAY(a)	{ outb(DAY, CMD); \
				  a = inb(DATA); \
				}
#define		READ_DATE(a)	{ outb(DATE, CMD); \
				  a = inb(DATA); \
				}
#define		READ_MON(a)	{ outb(MON, CMD); \
				  a = inb(DATA); \
				}
#define		READ_YEA(a)	{ outb(YEA, CMD); \
				  a = inb(DATA); \
				}
#define		WRITE_DATE(a)	{ outb(DATE, CMD); \
				  outb(a, DATA);\
				}


#define 	MAGIC_NUM 	's'

#define 	GET_SEC		_IOR(MAGIC_NUM,0,unsigned int)

#define 	GET_MIN		_IOR(MAGIC_NUM,1,unsigned int)

#define 	GET_HOU		_IOR(MAGIC_NUM,2,unsigned int)

#define 	GET_DAY		_IOR(MAGIC_NUM,3,unsigned int)

#define 	GET_DATE	_IOR(MAGIC_NUM,4,unsigned int)

#define 	GET_MON		_IOR(MAGIC_NUM,5,unsigned int)

#define 	GET_YEA		_IOR(MAGIC_NUM,6,unsigned int)

#define 	SET_DATE	_IOW(MAGIC_NUM,7,unsigned int)

#endif 
