#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>

int global;

int sum(void)
{
	int a, b, c;
	a = 10;
	b = 20;
	c = a + b;

	return c;
}

int init_module(void)
{
	pr_info("sum module is loaded\n");
	global = 20;
	return 0;
}

void cleanup_module(void)
{
	pr_info("export module is unloaded\n");
	return;
}
MODULE_AUTHOR("Achyuth sai");
MODULE_DESCRIPTION("EXPORT: exporting sum function to other modules with GPL license");
MODULE_LICENSE("GPL");

EXPORT_SYMBOL_GPL(global);
EXPORT_SYMBOL_GPL(sum);

