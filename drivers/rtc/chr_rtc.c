#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/version.h>
#include<asm/uaccess.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include"header.h"

ssize_t rtc_read (struct file *, char __user *, size_t, loff_t *);
ssize_t rtc_write (struct file *, const char __user *, size_t, loff_t *);
int rtc_open(struct inode *, struct file *);

int d, m, y, h, min, s;


int rtc_open(struct inode *inode, struct file *fi)
{
	printk(KERN_INFO "rtc_open successfully invoked\n");
	return 0;
}
struct rtc {
	int date;
	int month;
	int year;
	int hour;
	int minutes;
	int seconds;
};

struct rtc *buffer;

ssize_t rtc_write (struct file *fd, const char __user *buf, size_t a, loff_t *b)
{
	int *value;
	copy_from_user(value,buf,1);
	//int *value = (int *)(buf);
	int num = *value;
	int read;

	switch(num) {
			case 0: READ_DATE(read)
				struct rtc obj1 = { .date = read };
				buffer =  &obj1;
				goto over;
			case 1: READ_MONTH(read)
				struct rtc obj2 = { .date = read };
				buffer = (struct rtc *)&obj2;
				goto over;
			case 2: READ_YEAR(read)
				struct rtc obj3 = { .date = read };
				buffer = (struct rtc *) &obj3;
				goto over;
			case 3: READ_HOUR(read)
				struct rtc obj4 = { .date = read };
				buffer = (struct rtc *)&obj4;
				goto over;
			case 4: READ_MIN(read)
				struct rtc obj5 = { .date = read };
				buffer = (struct rtc *) &obj5;
				goto over;
			case 5: READ_SEC(read)
				struct rtc obj6 = { .date = read};
				buffer = (struct rtc *) &obj6;
				goto over;
			case 6: READ_DATE(d)
				READ_MONTH(m)
				READ_YEAR(y)
				READ_HOUR(h)
				READ_MIN(min)
				READ_SEC(s)
				goto over;
			default: printk(KERN_ALERT " Error while reading buffer\n");
				 break;
			};
			struct rtc obj;
			obj.date = d;
			obj.month = m;
			obj.year = y;
			obj.hour = h;
			obj.minutes = min;
			obj.seconds = s;
	
			buffer = ( struct myrtc *) &obj;
			goto over;
		over :	
		printk(KERN_INFO "rtc charater driver invoked successfully\n");

		return 0;
	
}

ssize_t rtc_read (struct file *in, char __user *buff, size_t a, loff_t *off)
{
		
	copy_to_user(buff, buffer, 17);
	printk(KERN_INFO "rtc_read call invokde\n");
	return 0;
}

struct file_operations fops = { .owner = THIS_MODULE,
				.open = rtc_open,
				.read = rtc_read,
				.write = rtc_write,
			};


int init_module(void)
{

	int ret;

	ret = register_chrdev(300, "RTC_CHR", &fops);
	if(ret<0){
		printk(KERN_ALERT "registration fails\n");
		return 0;
		}
	printk(KERN_INFO "rtc_driver inserted\n");
	return 0;
}

void cleanup_module(void)
{
	unregister_chrdev(300, "RTC_CHR");
	printk(KERN_INFO "rtc_driver unregisterd\n");
	return;
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SAI");


