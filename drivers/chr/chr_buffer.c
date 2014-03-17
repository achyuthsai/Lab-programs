#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/uaccess.h>

#define MAJR 300
#define MINR 0
#define DR_NAME "char_statically"


static dev_t mydev;
static struct cdev *mycdev;
static char kbuf[1024]; /* kernel buffer*/
static int flag = 0; // validation flag for synchronization

int myopen(struct inode *inode, struct file *file)
{

	printk(KERN_INFO "read call has invoked successfully\n");
	return 0;
}

ssize_t myread(struct file *file, char __user *ubuf, size_t length, loff_t *offset)
{
	int ret;
	printk(KERN_INFO "read call has invoked successfully\n");

	if(length > 1024)
		length = 1024;

	ret = copy_to_user(ubuf, kbuf+(*offset), length);
	if(ret)
		return -EIO;

	
	return length;
}

ssize_t mywrite(struct file *file, const char __user *ubuf, size_t length, loff_t *offset)
{
	int ret;
	char *pos_buf;
	printk(KERN_INFO " write call has invoked successfully\n");
	pos_buf = *offset + kbuf;
	
	if(*offset >= 1024)
		return -1;

	if(length > 1024)
		return -EIO;

	ret = copy_from_user(pos_buf, ubuf, length);
	if(ret)
		return -EIO;

	*offset += length;

	printk("offset position :%lu\n", *offset);

	return length;
}


loff_t mylseek(struct file *file, loff_t length , int num)
{
	loff_t offset;
	pr_info("lseek invokded successfully\n");

	offset = file -> f_pos;
	pr_info("offset is at %lu\n", offset);

	switch(num){
		case 0: offset = length;
			break;

		case 1: offset += length;
			break;

		case 2: offset = offset - length;
			break;
	}

	return offset;
}

int myrelease(struct inode *node, struct file *fil)
{
	flag = 0;
	printk(KERN_INFO "released chr driver\n");
	return 0;
}

struct file_operations fops = {  .owner= THIS_MODULE,
						.open = myopen,
						.read = myread,
						.write = mywrite,
						.release = myrelease,
						.llseek = mylseek,
					};
static struct class *veda_class;
int init_module(void)
{
	mydev = MKDEV(MAJR,MINR);

	register_chrdev_region(mydev,1,DR_NAME);

	mycdev = cdev_alloc();
	cdev_init(mycdev, &fops);

	cdev_add(mycdev, mydev, 1);

	veda_class = class_create(THIS_MODULE, "VIRTUA");
	device_create(veda_class, NULL, mydev, NULL, "%s", "vbuff");

	printk(KERN_INFO "char_statically driver successfully registerd\n");

	return 0;
}

void cleanup_module(void)
{
	cdev_del(mycdev);
	unregister_chrdev_region(mydev, 1);

	device_destroy(veda_class, mydev);
	class_destroy(veda_class);

	printk(KERN_INFO " char_statically driver successfully unregisterd\n");
	return;
}


MODULE_AUTHOR("Achyuth sai");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("char_statically driver implementation");
