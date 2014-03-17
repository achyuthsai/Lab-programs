#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/version.h>
#include<linux/cdev.h>
#include<linux/timer.h>
#include<linux/io.h>


#define GPBCON     0x56000010    //Configures the pins of port B                 0x00
#define GPBDAT     0x56000014    // The data register for port B                  USERDATA
#define GPBUP      0x56000018    // pull-up disable register for port B           0x00


void *gpbcon, *gpbdat, *gpbup;
struct timer_list my_timer1, my_timer2;
int flag1 = 0;
int flag2 = 0;


void off_state(unsigned long a)
{
	if(flag1 < 20) {
	pr_info("off_state funcntion invokde\n");
	iowrite32((1<<8),gpbdat);
	
	mod_timer(&my_timer1, jiffies+(2*HZ));

	flag1 ++;
	}

	return;
}

void on_state(unsigned long a)
{
	if(flag2 < 20) {
	pr_info("on_state function invokde\n");
	
	iowrite32(0x00, gpbdat);
	mod_timer(&my_timer2, jiffies+(2*HZ));

	flag2 ++;
	}

	return;
}
int init_module(void)
{
	init_timer(&my_timer1);
	init_timer(&my_timer2);

	my_timer1.function = off_state;
	my_timer1.expires = jiffies +(2*HZ);
	my_timer1.data = 0;

	my_timer2.function = on_state;
	my_timer2.expires = jiffies +(2*HZ);
	my_timer2.data = 0;
	

	gpbcon = ioremap_nocache(GPBCON, sizeof(int)*3);

	gpbdat = gpbcon + 0x04;
	gpbup = gpbdat + 0x04;

	iowrite32(0x00, gpbup);

	iowrite32(0x00, gpbcon);

	iowrite32((1 << 16), gpbcon);

	iowrite32(0x00, gpbdat);

	add_timer(&my_timer1);
	add_timer(&my_timer2);

	pr_info("module inserted\n");
	return 0;
}

void cleanup_module(void)
{
	iowrite32(1<<8, gpbdat);
	iowrite32((1 << 8), gpbup);

	pr_info("module unloaded\n");
	return;
}

	

	 
	

