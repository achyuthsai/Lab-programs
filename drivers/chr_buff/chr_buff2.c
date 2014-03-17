#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/fs.h>
#include<linux/path.h>
#include<linux/dcache.h>

#define MAJR 350
#define MINR 0
#define DEV_NAME "chr_minor2"

char DEVBUFA[1024];  	// kernel buffer 1
char DEVBUFB[1024]; 	// kernel buffer 2


dev_t mydev;
struct cdev *mycdev;

ssize_t min_open(struct inode *node, struct file *fip)
{
	pr_info("open call invoked successfully\n");
	return 0;
}

ssize_t min_read(struct file *fip, char __user *buf, size_t length, loff_t *offset)
{
	int minor;
	minor = iminor(fip->f_path.dentry->d_inode);
	pr_info("read call invoked with %d minor\n", minor);
	return 0;
}

struct file_operations fops = { .owner = THIS_MODULE,
			 	.open = min_open,
				.read = min_read,
			};


int init_module(void)
{
	mydev = MKDEV(MAJR, MINR);
	register_chrdev_region(mydev, 2, DEV_NAME);

	mycdev = cdev_alloc();

	cdev_init(mycdev, &fops);
	cdev_add(mycdev, mydev, 2);

	pr_info("buffer2 module is inserted\n");

	return 0;
}

void cleanup_module(void)
{
	cdev_del(mycdev);
	unregister_chrdev_region(mydev, 2);

	pr_info("buffer2 module is unloaded\n");

	return;
}

MODULE_AUTHOR("Achyuth sai");
MODULE_LICENSE("GPL");





