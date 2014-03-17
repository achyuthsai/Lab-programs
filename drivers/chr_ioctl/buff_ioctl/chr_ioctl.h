#ifndef chr_ioctl
#define chr_ioctl 


#include<asm/ioctl.h>

#define MAX 		 4096
#define MAGIC_NUM 	's'

#define WRITE_DATA 	_IOW('s', 1, char *)

#define READ_DATA	_IOWR('s', 2, char *)

typedef struct buff_data {
		char name[MAX];
		int length;
		int seek;
		int position;

} buffer_t ;

		
#endif
