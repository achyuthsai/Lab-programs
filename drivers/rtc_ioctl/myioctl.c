#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/slab.h>
#include<linux/device.h>
#include<linux/ioctl.h>
#include"header.h"


int my_open(struct inode *nod, struct file *fi)
{

	pr_info("myopen is invokded\n");
	return 0;
}

long my_ioctl(struct file *fi, unsigned int cmd, unsigned long arg)
{

	int s,min,h,d,m,y;
	int *sec,*minu, *hour, *date, *month, *year, *date1;
	if(_IOC_TYPE(cmd)!= MAGIC_NUM) {
			return -ENOTTY;
		}

	switch(cmd) {
		
		case GET_YEA:if(_IOC_DIR(cmd) & _IOC_READ){
					if(!(access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd))))
							return -EIO;
				}
				pr_info("GET_YEA cmd invoked\n");
				READ_YEA(y);
				pr_info("year: %x\n", y);
				year = &y;
				copy_to_user((int *)arg,year,4);
				break;

		case GET_MON:if(_IOC_DIR(cmd) & _IOC_READ){
					if(!(access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd))))
							return -EIO;
				}
				pr_info("GET_MON cmd invoked\n");
				READ_MON(m);
				pr_info("year: %x\n", m);
				month = &m;
				copy_to_user((int *)arg,month,4);
				break;
		case GET_DATE:if(_IOC_DIR(cmd) & _IOC_READ){
					if(!(access_ok(VERIFY_WRITE, (void *) arg, _IOC_SIZE(cmd))))
							return -EIO;
				}
				pr_info("GET_DATE cmd invoked\n");
				READ_DATE(d);
				pr_info("year: %x\n", d);
				date = &d;
				copy_to_user((int *)arg,date,4);
				break;
		
		case SET_DATE:if(_IOC_DIR(cmd) & _IOC_WRITE){
					if(!(access_ok(VERIFY_READ, (void *) arg, _IOC_SIZE(cmd))))
							return -EIO;
				}
				pr_info("SET_DATE cmd invoked\n");
				copy_from_user(&d,(int *)arg,4);
		//		d = *date1;
				WRITE_DATE(d);
				pr_info("date: %x\n", d);
				break;
 
		default : return -1;
	}

	return 0;
}
				

struct file_operations fops = { .owner = THIS_MODULE,
				.open = my_open,
				.unlocked_ioctl = my_ioctl,
				};

struct class *rtc_class;

dev_t mydev;
int major;
int init_module(void)
{

	major = register_chrdev(0, "ioctl_rtc", &fops);

	mydev = MKDEV(major,0);

	if(!major){
		printk(KERN_ALERT "registration failed\n");
		return -1;
	}

	rtc_class = class_create(THIS_MODULE, "VIRTUAL");
	device_create(rtc_class,NULL, mydev, NULL, "%s","rtc_ioctl");

	pr_info("module inserted\n");

	return 0;
}

void cleanup_module(void)
{

	unregister_chrdev(major,"ioctl_rtc");
	device_destroy( rtc_class,mydev);
	class_destroy(rtc_class);
	pr_info("module unloaded\n");

	return;
}

MODULE_AUTHOR("SAI");
MODULE_LICENSE("GPL");	
