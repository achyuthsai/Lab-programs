#ifndef chr_ioctl
#define chr_ioctl 


#include<asm/ioctl.h>
#define MAGIC_NUM 	's'

#define FILL_ZERO 	_IO('s', 1)

#define FILL_CHAR	_IOW('s', 2, char)

#define SET_SIZE	_IOW('s', 3, unsigned int)

#define GET_SIZE	_IOR('s', 4, unsigned int)

#endif
