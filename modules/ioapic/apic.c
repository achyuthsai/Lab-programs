#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<asm/io.h>
#include<asm/io_apic.h>
#include<asm/iomap.h>

#define PHY_ADR 0xfec00000
#define RANGE   1024
int init_module(void)
{
	int a;
	int *regsel;
	int *regwin;
	void *apic;

	apic = ioremap_nocache(PHY_ADR, RANGE);

	regsel = apic;
	regwin = apic + 0x10;

//	iowrite(0, regsel);
	*regsel = 0x01;
	a = *regwin; 
	printk("APIC id is %x", a);

	return 0;
}

void cleanup_module(void)
{
	printk("module unloaded");
	return;
}

MODULE_LICENSE("GPL");

