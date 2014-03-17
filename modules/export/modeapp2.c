#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>

int a;
int *b;
int (*fp)(void);
int init_module(void)
{
	pr_info("modeapp2 module is loaded\n");
	fp = __symbol_get("sum");
	a = (*fp)();
	b = __symbol_get("global");
	*b = 500;
	 __symbol_put("sum");
	__symbol_put("global");
	printk("return value of sum %d\n", a);
	printk("global value is changed to  %d\n", *b);
	return 0;
}

void cleanup_module(void)
{
	pr_info("modeapp2 module is unloaded\n");
	return;
}

MODULE_AUTHOR("Achyuth sai");
MODULE_DESCRIPTION("accessing sum module indirectly");
MODULE_LICENSE("GPL");


	
