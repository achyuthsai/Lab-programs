#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/sched.h>

struct thread_info *td;
struct task_struct *pcb;

int init_module(void)
{
	td = current_thread_info();
	pcb = td->task;

	printk("current process pid is %u\n", pcb->pid);
	printk("current process address space is  %p\n", pcb->mm);

	return 0;
}

void cleanup_module(void)
{
	pr_info("pcb module unloaded\n");
	return;
}

MODULE_AUTHOR("Achyuth sai");
MODULE_DESCRIPTION("Accessig pid of current process");
MODULE_LICENSE("GPL");

