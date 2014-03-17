#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>

int date;
int month;
int year;

int init_module(void)
{

	outb(0x07, 0x70);
	date = inb(0x71);
	outb(0x08, 0x70);
	month = inb(0x71);
	outb(0x09, 0x70);
	year = inb(0x71);

	
	outb(0x04, 0x70);
	printk("Time %x ",inb(0x71));
	outb(0x02, 0x70);
	printk(":%x ",inb(0x71));
	outb(0x00, 0x70);
	printk(":%x \n",inb(0x71));

	printk("date: %x/%x/%x\n", date, month, year);

	return 0;
}

void cleanup_module(void)
{
	printk("rtc module is unloaded\n");
	return;
}

MODULE_AUTHOR("Achyuth sai");
MODULE_DESCRIPTION("Prints time and date");
MODULE_LICENSE("GPL");


