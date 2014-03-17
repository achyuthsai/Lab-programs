#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>

#define MAJR 300
#define MINR 0
#define DR_NAME "char_statically"


static dev_t mydev;
static struct cdev *mycdev;

ssize_t myread(struct file *file, char __user *buf, size_t a, loff_t *lof)
{
	printk(KERN_INFO "read call has invoked successfully\n");
	return 0;
}
ssize_t mywrite(struct file *file, const char __user *buf, size_t a, loff_t *b)
{

	printk(KERN_INFO " write call has invoked successfully\n");
	return 0;
}

int myopen(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "read call has invoked successfully\n");
	return 0;
}

struct file_operations fops = {  .owner= THIS_MODULE,
						.open = myopen,
						.read = myread,
						.write = mywrite,
					};
int init_module(void)
{
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
	cdev_del(mycdev);
	unregister_chrdev_region(mydev, 1);

	printk(KERN_INFO " char_statically driver successfully unregisterd\n");
	return;
}


MODULE_AUTHOR("Achyuth sai");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("char_statically driver implementation");
