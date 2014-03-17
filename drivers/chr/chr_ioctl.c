#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#include"chr_ioctl.h"
#include<linux/slab.h>


#define MAJR 300
#define MINR 0
#define DR_NAME "char_ioctl"


static dev_t mydev;
static struct cdev *mycdev;

struct pr_data {
	char *kbuf;
	unsigned int size;
};

struct pr_data obj1;


ssize_t myread(struct file *file, char __user *buf, size_t a, loff_t *lof)
{
	int ret;
	printk(KERN_INFO "read call has invoked successfully\n");
	ret = copy_to_user(buf,obj1.kbuf,a);
	if(ret<0){
		printk(KERN_ALERT "copy_from_user failed");
		return -1;
		}
	return 0;
}
ssize_t mywrite(struct file *file, const char __user *buf, size_t a, loff_t *b)
{
	int ret;
	int size = 20;
	ret = copy_from_user(obj1.kbuf,buf,size);
	if(ret<0){
		printk(KERN_ALERT "copy_from_user failed");
		return -1;
		}
	printk(KERN_INFO "written into buffer: %s", obj1.kbuf);

	printk(KERN_INFO " write call has invoked successfully\n");
	return 0;
}

int myopen(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "open call has invoked successfully\n");
	return 0;
}

static long myioctl (struct file *fi, unsigned int cmd, unsigned long arg)
{
	int *p = (int *) arg;

	pr_info("chr_ioctl invokde successfully\n");

	switch(cmd) {

		case FILL_ZERO: 
				memset(obj1.kbuf,'0' , obj1.size);
				pr_info("%s\n", obj1.kbuf);
				break;
	
		case FILL_CHAR: 
				memset(obj1.kbuf,arg, obj1.size);
				break;

		case SET_SIZE: pr_info("SET_SIZE invokde\n");
				obj1.size = arg;
				krealloc(obj1.kbuf, arg, GFP_KERNEL);
				pr_info("size: %d\n", obj1.size);
				break;
		case GET_SIZE: *p = obj1.size;
				break;
		default: 
			return -1;
	}
	return 0;
}

struct file_operations fops = {  .owner= THIS_MODULE,
						.open = myopen,
						.read = myread,
						.write = mywrite,
						.unlocked_ioctl = myioctl,
					};
int init_module(void)
{
	obj1.size = 1024;
	obj1.kbuf = kmalloc(obj1.size, GFP_KERNEL);

	mydev = MKDEV(MAJR,MINR);

	register_chrdev_region(mydev,1,DR_NAME);

	mycdev = cdev_alloc();
	cdev_init(mycdev, &fops);

	cdev_add(mycdev, mydev, 1);

	printk(KERN_INFO "char_statically driver successfully registerd\n");

	return 0;
}

void cleanup_module(void)
{
	kfree(obj1.kbuf);
	cdev_del(mycdev);
	unregister_chrdev_region(mydev, 1);

	printk(KERN_INFO " char_statically driver successfully unregisterd\n");
	return;
}


MODULE_AUTHOR("Achyuth sai");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("char_statically driver implementation");
