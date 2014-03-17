/* eeprom.c - A small char driver to work with mini2440 eeprom
 *
 * Author: Veda Solutions (www.techveda.org)
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/cdev.h>


#define EEPROM_SIZE 256 // size of mini2440 eeprom

/*driver private data*/
struct eeprom_data {
	struct mutex lock;  // lock to protect access from multiple tasks
	struct i2c_client *client; // i2c device pointer*/
	struct class *cls;  //pointer to a virtual class in sysfs
	struct cdev cdev;  // our char device object
	dev_t devno;       // device numbe (includes major and minor)
};


/*
 * Will be called when our char device "myeeprom" is opened
 * using 'open'
 * 
 * Inputs- inode - inode object for our char device file
 *         file  - file pointer for our opened char device
 *
 * return 0 on success, negative value on failure
 */
static int eeprom_open(struct inode *inode, struct file *file)
{
	/*
	 * driver data contains cdev object, as we know cdev 
	 * address from the inode, use this to get the pointer
	 * to our private data, and store it in the file->private_data
	 * which can be used later in read, write, and release
	 */	
	pr_info("open invoked \n");
	struct eeprom_data *ed = container_of(inode->i_cdev,
				 struct eeprom_data, cdev);

	file->private_data = ed;
	pr_info("client is %p",ed->client);
        return 0;

}

/* Will be called when our char device "myeeprom" is closed*/ 
static int eeprom_release(struct inode *inode, struct file *file)
{
	file->private_data = NULL;
        return 0;
}

/*
 * Called when a read operation is performed on our char device
 * "myeeprom"
 *
 * Inputs: file - opened file object
 *         buf  - Pointer to user space memory where data will be 
 *                written to
 *         count - number bytes to read
 *         off   - pointer to the file offset
 * 
 * Return number of bytes read on success, a negative number
 * on failure
 */
static ssize_t eeprom_read(struct file *fd, unsigned char __user *buf, size_t count,
                            loff_t * off)
{
	struct eeprom_data *ed = (struct eeprom_data *)fd->private_data;
	int ret = 0;
	char *kbuf; // kernel space memory to be passed to i2c-core
	unsigned char msgbuf = 0;  // contains offset 
	/*for debug information*/
	/*-------------------------------------------------------*/
	pr_info("in read client add is %p\n",ed->client);
	pr_info("read invoked and offset is %d\n",*off);
	/*--------------------------------------------------------*/
	/*zero count!*/
	if (unlikely(!count))
                return count;

	/*data to be read is more than what we can read, adjust it*/
	if (*off+count > EEPROM_SIZE) 
		count = EEPROM_SIZE - *off;

	/*hold lock to protect from other tasks*/
        mutex_lock(&ed->lock);
	kbuf = kzalloc(count, GFP_KERNEL);
	if (!kbuf) {
		pr_info("Counld not allocate memory for read\n");
		mutex_unlock(&ed->lock);
		return -ENOMEM;
	}

	/*first tell offset info to the device*/
//	msgbuf = *off;  // assuming this eeprom supports 1 byte offset 
	ret = i2c_master_send(ed->client, off, 1);
	if (ret <= 0) {
		pr_info("Error writing offset info to the device  %d\n",ret);
		mutex_unlock(&ed->lock);
		return ret;
	} 
	
	/* Now Issue the read request to the i2c-core*/
	ret = i2c_master_recv(ed->client, kbuf, count);	
	if (ret < 0) {
		pr_info("Read failed %d\n", ret);
		mutex_unlock(&ed->lock);
		return ret;
	}
	
	/*copy the received data to the user space*/
	copy_to_user(buf, kbuf, count);

	/*update file offset*/
	*off += count;

	kfree(kbuf);
        mutex_unlock(&ed->lock);

	
	return count;
}

/*
 * Called when a write operation is performed on our char device
 * "myeeprom"
 *
 * Inputs: file - opened file object
 *         buf  - Pointer to user space memory from where data will be 
 *                written to device
 *         count - number bytes to write
 *         off   - pointer to the file offset
 * 
 * Return number of bytes written on success, a negative number
 * on failure
 */
static ssize_t eeprom_write(struct file *fd, const char  __user *buf, size_t count,
                             loff_t * offset)

{
	 struct eeprom_data *ed = (struct eeprom_data *)fd->private_data;
        int ret;
	char *kbuf;
	int off=0;
		/* for debug information*/
		/*------------------------------------------------------*/
		pr_info("write invoked and offset is %d\n",*offset);
		/*-----------------------------------------------------*/
	/*Sanity check on size*/
	if (off+count > EEPROM_SIZE)
		return -ENOSPC;


	mutex_lock(&ed->lock);
	/*allocate memory for offset and data*/
	kbuf = kzalloc(count+1, GFP_KERNEL);
	if (!kbuf) {
		pr_info("Error allocating memory\n");
		mutex_unlock(&ed->lock);
		return -ENOMEM;
	}

	/*first fill in the offset to where we want write*/
	kbuf[0] = off;
	/*-----we can use memdup_user instead of copy_from_user------*/
	//kbuf = memdup_user(buf, count);
	/*-------------------------------------------------------------*/
        copy_from_user(kbuf+1, buf, count);

	/*--checking whether copy from user is successful or not--*/
	pr_info("data copied from user is %s\n",kbuf);
	/*--------------------------------------------------------*/

	/*Send data to i2c-core to i2c eeprom*/
        ret = i2c_master_send(ed->client, kbuf, count+1);
        if(ret <= 0) 
           pr_info("i2c send failed\n");
	else
		off += count; //update the file offset

	
	kfree(kbuf);
	mutex_unlock(&ed->lock);

        return ret;
}

/*set file offset to desired position*/
loff_t eeprom_seek(struct file *file, loff_t offset, int orig)
{
        loff_t ret;

        switch (orig) {
        case SEEK_CUR:
                offset += file->f_pos;
        case SEEK_SET:
		/*check overflow and underfolow*/
                if (offset >= EEPROM_SIZE) {
                        ret = -EOVERFLOW;
                        break;
                } else if (offset < 0) {
			ret = -EINVAL;
			break;
		}

                file->f_pos = offset;
                ret = file->f_pos;
                break;
        default:
                ret = -EINVAL;
        }
        return ret;
}

/* char dev operations*/
static const struct file_operations eeprom_ops = {
        .owner = THIS_MODULE,
        .read = eeprom_read,
        .write = eeprom_write,
        .open = eeprom_open,
	.llseek = eeprom_seek,
        .release = eeprom_release,
};

/*
 * Called by i2c-core when the i2c device with the address
 * 0x50 is found on the adapter.
 *
 * Inputs - client - i2c device pointer
 *          id     - the id that matched 
 * 
 * Return 0 on succefull probing, negative number
 * on failure
 */
static int eeprom_probe(struct i2c_client *client,
                        const struct i2c_device_id *id)
{
	struct eeprom_data *ed;
	int ret;
	pr_info("in probe client add is %p",client);
	/* Allocate memory for our device private data*/
	ed = kzalloc(sizeof(*ed), GFP_KERNEL);
	if (!ed) {
		pr_info("Could not allocat memory for driver data\n");
		return -ENOMEM;
	}

	ed->client = client;

	/* Allocate char device number*/
	ret = alloc_chrdev_region(&ed->devno, 0, 1, "eeprom_drv");
	if (ret < 0) {
		pr_info("Error getting device number\n");
		goto free_data;
	}

	/*initialize cdev and register with char subsytem*/
	cdev_init(&ed->cdev, &eeprom_ops);
	ret = cdev_add(&ed->cdev, ed->devno, 1);
	if (ret) {
		pr_info("error adding char device\n");
		goto unreg_chrdev_reg;
	}

	/* Create our virtual class */
        ed->cls = class_create(THIS_MODULE, "EEPROM");
        if (!ed->cls) {
                pr_info("Error creating class\n");
                ret = -ENOMEM;
                goto del_cdev;
        }

        /*create the device in our virtual class "myclass"*/
        if (!device_create(ed->cls, NULL, ed->devno,
                                 NULL, "eeprom_device")) {
                ret = -ENOMEM;
                goto delete_class;
        }

	/*
	 * Store our device private data in i2c_client object, so
	 * that we can retrieve it back in eeprom_remove
	 */
		i2c_set_clientdata(client, ed);
		//ed->client=client;	
	
	/*Initialize our lock*/
	mutex_init(&ed->lock);

	pr_info("Succesfully probed client %s\n", client->name);
        return 0;

delete_class:
        class_destroy(ed->cls);
del_cdev:
	cdev_del(&ed->cdev);
unreg_chrdev_reg:
        unregister_chrdev_region(ed->devno, 1);
free_data:
	kfree(ed);

        return ret;
}

/*Will be called from the i2c-core when our eeprom disappears*/
static int eeprom_remove(struct i2c_client *client)
{
	struct eeprom_data *ed;

	/*Get our device private data, see above i2c_set_clientdata*/
	ed = i2c_get_clientdata(client);

	/* Remove the 'myeeprom' device entry from /dev/ */
        device_destroy(ed->cls, ed->devno);
	/*Destroy our virtual class in sysfs*/
        class_destroy(ed->cls);

	/*Unregister with char subsytem*/
	cdev_del(&ed->cdev);
        unregister_chrdev_region(ed->devno, 1);

	/*free device private data*/
	kfree(ed);
	return 0;
}

/*Ask i2c-core for the device with address 0x50*/
static struct i2c_device_id eeprom_ids[] = {
        { "24c08",0x50 },  // mini2440 i2c device
	{ } // end of the list
};


static struct i2c_driver eeprom_driver = {
	.driver = {
		.name = "eeprom",
		.owner = THIS_MODULE,
	},
	.probe = eeprom_probe,
	.remove = eeprom_remove,
	.id_table = eeprom_ids,
};

/*module initialization routine*/
static int __init eeprom_init(void)
{
	/*register with i2c-core*/
	return i2c_add_driver(&eeprom_driver);
}
module_init(eeprom_init);

static void __exit eeprom_exit(void)
{
	i2c_del_driver(&eeprom_driver);
}
module_exit(eeprom_exit);

MODULE_DESCRIPTION("Driver for I2C EEPROM");
MODULE_AUTHOR("Veda Solutions");
MODULE_LICENSE("GPL");
