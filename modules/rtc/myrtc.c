#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/version.h>
#include"header.h"

extern int mycalender(void);
extern int mytime(void);

int init_module(void)
{
	mycalender();
	mytime();
	return 0;
}

void cleanup_module(void)
{
	pr_info("myrtc module unloaded\n");
	return;
}

MODULE_AUTHOR("Achyuth sai");
MODULE_DESCRIPTION("accessing functions module for rtc");
MODULE_LICENSE("GPL");


