#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/proc_fs.h>
#include"header.h"
#include<linux/mutex.h>

/* FUNCTION DECLARATIONS*/
/********************************************
extern struct proc_dir_entry *proc_mkdir(const char *,struct proc_dir_entry *);
typedef int (read_proc_t)(char *page, char **start, off_t off,
                          int count, int *eof, void *data);
typedef int (write_proc_t)(struct file *file, const char __user *buffer,
                           unsigned long count, void *data);


extern struct proc_dir_entry *create_proc_entry(const char *name, mode_t mode,
                                                struct proc_dir_entry *parent);

extern void remove_proc_entry(const char *name, struct proc_dir_entry *parent);

********************************************/
int date, month, year, hour, minute, sec;

struct mutex proc_lock;

int read_rtc_date(char *page, char **start, off_t off, int count, int *eof, void *data)
{
		int a;

		mutex_lock(&proc_lock);
		READ_DATE(date);
		mutex_unlock(&proc_lock);

		mutex_lock(&proc_lock);
		READ_MON(month);
		mutex_unlock(&proc_lock);

		mutex_lock(&proc_lock);
		READ_YEA(year);
		mutex_unlock(&proc_lock);

		pr_info("read_proc function is invoked\n");

		a = sprintf(page, "%x/%x/%x\n", date,month,year);	
		return a;
		
}

int read_rtc_time(char *page, char **start, off_t off, int count, int *eof, void *data)
{
		int a;

		mutex_lock(&proc_lock);
		READ_HOU(hour);
		mutex_unlock(&proc_lock);

		mutex_lock(&proc_lock);
		READ_MIN(minute);
		mutex_unlock(&proc_lock);

		mutex_lock(&proc_lock);
		READ_SEC(sec);
		mutex_unlock(&proc_lock);

		pr_info("read_proc function is invoked\n");

	/*	minute = minute + 0x30;
		hour = hour + 0x05;

		if(minute > 0x60)
			hour = hour + 0x01;
*/
		
		a = sprintf(page, "%x:%x:%x\n", hour,minute,sec);	
		mutex_unlock(&proc_lock);

		return a;
		
}

int write_rtc_date(struct file *file, const char __user *buffer, unsigned long count, void *data)
{
		int a;


		a = sscanf(buffer, "%x/%x/%x\n", &date, &month, &year);
	
		mutex_lock(&proc_lock);
		WRITE_DATE(date);
		mutex_unlock(&proc_lock);


		mutex_lock(&proc_lock);
		WRITE_MON(month);
		mutex_unlock(&proc_lock);


		mutex_lock(&proc_lock);
		WRITE_YEA(year);
		mutex_unlock(&proc_lock);

		pr_info("read_proc function is invoked\n");

		return a;
		
}

struct proc_dir_entry *dir1, *dir2, *dir3;

int init_module(void)
{

	mutex_init(&proc_lock);
	dir1 = proc_mkdir("rtc_proc", dir1);

	dir2 = create_proc_entry("rtc_date", 0666, dir1);
	dir3 = create_proc_entry("rtc_time", 0666, dir1);
	dir2->read_proc = read_rtc_date;
	dir2->write_proc = write_rtc_date;
	dir3->read_proc = read_rtc_time;

	pr_info("module inserted\n");

	return 0;
}

void cleanup_module(void)
{
	
	remove_proc_entry("rtc_time", dir1);
	remove_proc_entry("rtc_date", dir1);
	remove_proc_entry("rtc_proc4", NULL);

	pr_info("module removed\n");


	return;
}

MODULE_AUTHOR("SAI");
MODULE_LICENSE("GPL");

	


	

	
