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

int cmd[7] = {DATE,MONTH,YEAR,HOUR,MIN,SEC};
char kbuf[17];

int d,m,y,h,min,s;

int rtc_open(struct inode *inode, struct file *fi)
{
	printk(KERN_INFO "rtc_open successfully invoked\n");
	return 0;
}

struct rtc *buffer;

ssize_t rtc_write (struct file *fd, const char __user *buf, size_t a, loff_t *b)
{

	READ_DATE(d);
	READ_MONTH(m);
	READ_YEAR(y);
	READ_HOUR(h);
	READ_MIN(min);
	READ_SEC(s);

	min = min + 0x30;
	h = h + 0x05;

	if(min > 0x60){
		h = h + 0x01;
	}
	if( h > 0x24)
		d = d + 0x01;
	if( d > 0x31)
		m = m + 1;


		
	sprintf(kbuf,"%x:%x:%x:%x:%x:%x", d,m,y,h,min,s);
	pr_info("written calender in buffer\n");

	return 0;
	
}

ssize_t rtc_read (struct file *in, char __user *buff, size_t a, loff_t *off)
{
	int ret;
	char *pos;

	pos = (*off)+kbuf;

	ret = copy_to_user(buff, pos, a);
	printk(KERN_INFO "rtc_read call invokde\n");
	return 0;
}

loff_t rtc_lseek(struct file *fi, loff_t length, int num)
{
	loff_t offset;

	offset = fi->f_pos;

	if(length > 12)
		return EIO;

	switch(num) {
		case 0: offset = length;
			break;
		case 1: offset += length;
			break;
		default: pr_info("error with lseek\n");
			break;

		};

	if(offset < 0)
		return EIO;

	fi->f_pos = offset;

	return offset;
}


struct file_operations fops = { .owner = THIS_MODULE,
				.open = rtc_open,
				.read = rtc_read,
				.write = rtc_write,
				.llseek = rtc_lseek,
			};


int init_module(void)
{

	int ret;

	ret = register_chrdev(350, "RTC_CHR", &fops);
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


