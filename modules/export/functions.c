#include"header.h"
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>


int mycalender(void)
{
	int a,d,m,y;

	 READ_DAY(a);
	
	switch(a){
		case 0: printk("sun ");
			break;
		
		case 1: printk("Mon ");
			break;
		case 2: printk("Thu ");
			break;
		case 3: printk("Wed ");
			break;
		case 4: printk("Thu ");
			break;
		case 5: printk("Fri ");
			break;
		case 6: printk("Sat ");
			break;
		default:printk("date not availabale");
			break;
	}
	READ_DATE(d);
	READ_MONTH(m);
	READ_YEAR(y);

	printk("%x/%x/%x\n", d, m, y);

	return 0;
}

int mytime(void)
{
	int  s, m, h;

	READ_SEC(s);
	READ_MIN(m);
	READ_HOUR(h);

	printk("Time  %x:%x:%x\n",h,m,s);
	
	return 0;
}

int init_module(void)
{
	pr_info("functions module is loaded\n");
	return 0;
}

void cleanup_module(void)
{
	pr_info("functions module is unloaded\n");
	return;
}
EXPORT_SYMBOL_GPL(mycalender);
EXPORT_SYMBOL_GPL(mytime);
MODULE_AUTHOR("Achyuth sai");
MODULE_DESCRIPTION(" contains calender, time functions for rtc");
MODULE_LICENSE("GPL");


