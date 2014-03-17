#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>

int function(void)
{
	printk("hello world\n");
	return 0;
}


int init_module(void)
{
	//printk("module loaded\n");
	pr_info("module loaded\n");
	//printk(KERN_INFO "module loaded");

	return 0;
}


void cleanup_module(void)
{
	//printk("moduele unloaded");

	pr_info("module unloaded");
	// printk("KERN_INFO "module unloaded");

}

MODULE_AUTHOR("Achyuth");
MODULE_DESCRIPTION("first moduele");
MODULE_LICENSE("GPL");
