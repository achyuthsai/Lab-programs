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
	int minor, ret;
	char *kbuf;
	char *pos;
	minor = iminor(fip->f_path.dentry->d_inode);
	pr_info("read call invoked with %d minor\n", minor);

	if((length > 1024) | ((*offset) >= 1024))
		return EIO;

	switch(minor){
	
		case 0: kbuf = DEVBUFA;
			break;
		case 1: kbuf = DEVBUFB;
			break;
		default: pr_info("Error with minor number\n");
			return EIO;

		};

	pos = kbuf + (*offset);

	ret = copy_to_user(buf, pos, length);

	if(ret){
		pr_alert("%d bytes were failed to read\n", minor);
		return EIO;
	}

	return length;
}

ssize_t min_write(struct file *fip, const char __user *buf, size_t length, loff_t *offset)
{

	int  ret, minor;
	char *kbuf, *pos;

	minor = iminor(fip->f_path.dentry->d_inode);
	pr_info("write call invoked with %d minor\n", minor);

	if(length > 1024 | (*offset) >=1024)
		return EIO;

	switch(minor) {
		case 0: kbuf = DEVBUFA;
			break;
		case 1: kbuf = DEVBUFB;
			break;
		default: pr_info("Error with minor number\n");
			return EIO;

		};
	
	pos = kbuf + (*offset);

	ret = copy_from_user(pos, buf, length);

	if(ret) {
		pr_info("%d bytes were failed to write\n", ret);
		return EIO;
	}
	(*offset) += length;

	return length;
}
				 
loff_t min_lseek(struct file *fip, loff_t length, int num)
{
	loff_t offset;
	int minor;
	char *kbuf;

	offset = fip->f_pos;

	if(length > 1024)
		return EIO;
	minor = iminor(fip->f_path.dentry->d_inode);
	switch(minor) {
		case 0: kbuf = DEVBUFA;
			break;
		case 1: kbuf = DEVBUFB;
			break;
		default: pr_info("Error with minor number\n");
			return EIO;

		};

	switch(num) {
		case 0: offset = length;
			break;
		case 1: offset = (offset + length);
			break;
		case 2: offset = offset - length;
			break;
		default: pr_info("error with offset\n");
			break;
	};
	if(offset < 0 | offset > 1024)
		return EIO;

	fip->f_pos = offset;

	return offset;
}
struct file_operations fops = { .owner = THIS_MODULE,
			 	.open = min_open,
				.read = min_read,
				.write = min_write,
				.llseek = min_lseek,
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





