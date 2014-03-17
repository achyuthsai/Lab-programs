#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<asm/io.h>
#include<asm/io_apic.h>
#include<asm/iomap.h>

#define PHY_ADR 0xfec00000
#define RANGE   1024
#define PRINT_BITS(a) { int i; \
			for(i=31; i>=0; i--)\
				printk("%d ", (a >> i) & 1);\
			printk("\n");\
			}
#define PRINT_HEX(a) {\
			int i; \
			static int tmp = 1;\
			if(tmp % 2)\
			printk("0x");\
			for(i=7; i>=0; i--)\
				printk("%x", (a >> (i*4)) & 0xf);\
			if(!(tmp % 2))\
			printk("\n");\
			tmp ++;\
			}
int a;
int b;
int c;

int init_module(void)
{
	int i, ret;
	int *regsel = NULL;
	int *regwin = NULL;
	void *apic = NULL;
	int arr[24] = {0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 
		       0x20, 0x22, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e,
		       0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e};

	apic = ioremap_nocache(PHY_ADR, RANGE);

	regsel = apic;
	regwin = apic + 0x10;

	 iowrite32(0, regsel);
//	*regsel = 0;
//	a = *regwin; 
	a = ioread32(regwin);

	a = a & 0x0f000000 >> 23;
	printk("APIC id is %x\n", a);

	iowrite32(0x01, regsel);
	b = ioread32(regwin);
	c = b;
	b = b & 0x000000ff;
	printk("Version: %x\n", b);
	c = c & 0x00ff0000;
	c = c >> 16;
	printk("Redirection I/O : %x\n", c );
	printk("I/O Redirection table registers:\n");

	for(i=0x10; i<=0x3f; i+=1)
	{
		iowrite32(i,regsel);
		ret = ioread32(regwin);
		PRINT_HEX(ret);
	}	
	
	return 0;
}

void cleanup_module(void)
{
	printk("module unloaded\n");
	return;
}

MODULE_LICENSE("GPL");

