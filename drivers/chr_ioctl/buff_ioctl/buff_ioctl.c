#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/module.h>
#include<linux/cdev.h>
#include<linux/fs.h>
#include<asm/uaccess.h>
#include"chr_ioctl.h"
#include<asm-generic/ioctl.h>
#include<linux/slab.h>
#include<linux/string.h>

#define MAJR 300
#define MINR 0
#define DR_NAME "buff_ioctl"


static dev_t mydev;
static struct cdev *mycdev;

struct pr_data {
	char *kbuf;
	unsigned int size;
};

struct pr_data obj1;

int myopen(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "open call has invoked successfully\n");
	return 0;
}

static long myioctl (struct file *fi, unsigned int cmd, unsigned long arg)
{
	int size;
	loff_t offset;
	char *pos;
	buffer_t ubuf;

	if(_IOC_TYPE(cmd) != MAGIC_NUM) {
			return -ENOTTY;
	}


	pr_info("buff_ioctl invokde successfully\n");

	switch(cmd) {	
		case WRITE_DATA: 
				if(_IOC_DIR(cmd) & _IOC_WRITE){
					if(!(access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd))))
						return -EIO;
				}

				pr_info("WRITE_DATA invokde\n");
				offset = fi->f_pos;
				pos = obj1.kbuf + offset;

				copy_from_user(&ubuf, (buffer_t *)arg, sizeof(ubuf));
				size = ubuf.length;
				
				strcpy(pos, ubuf.name);

				offset = offset + size;
				fi->f_pos = offset;
				pr_info("data: %s", obj1.kbuf);
				break;

		case READ_DATA: if(_IOC_DIR(cmd) & (_IOC_READ | _IOC_WRITE)){
					if(!access_ok(VERIFY_WRITE | _IOC_READ, (void *)arg, _IOC_SIZE(cmd)))
						return -EIO;
				}
	
				pr_info("READ_DATA invokde\n");

				offset = fi->f_pos;

				copy_from_user(&ubuf, (buffer_t *)arg, sizeof(ubuf));
				
				switch(ubuf.seek) {
					case SEEK_SET : offset = ubuf.position;
							break;
					case SEEK_CUR: offset += ubuf.position;
							break;
					case SEEK_END: offset -= ubuf.position;
							break;
					default: return -EIO;
					}
				strcpy(ubuf.name, (obj1.kbuf+offset));
				ubuf.length = obj1.size;
				
				copy_to_user((buffer_t *)arg, &ubuf, sizeof(ubuf));
				
				break;
		default: 
			return -1;
	}
	return 0;
}

struct file_operations fops = {  .owner= THIS_MODULE,
						.open = myopen,
						.unlocked_ioctl = myioctl,
					};
int init_module(void)
{
	obj1.size = 4096;
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
