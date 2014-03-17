#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>

extern int sum(void);
int a;

int myinit(void)
{
	pr_info("modeapp1 module loaded\n");
	a = sum();
	printk("a : %d\n", a);
	return 0;
}

void myexit(void)
{
	pr_info("modeapp1 unloaded\n");
	return;
}
	
module_init(myinit);
module_exit(myexit);

MODULE_AUTHOR("Achyuth sai");
MODULE_DESCRIPTION("accessing return value of sum module");
MODULE_LICENSE("GPL");


