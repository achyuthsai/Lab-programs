/* Shows how to use malloc lib interface to allocate dynamic memory
Version : 1.0
Author : Team -C
*/

#include <malloc.h>
#include <stdio.h>

struct A {
	int a,b;
};

void main()
{
	int ret;
	int *ptr_int;
	short int *ptr_shortint;
	struct A *ptr_struct;
	
	ptr_int = (int *) malloc(sizeof(int));	/* Allocating 4 bytes of memory */	
	*ptr_int = 10;
	
	ptr_shortint = (short int *) malloc(sizeof(short int));
	*ptr_shortint = 20;

	ptr_struct = (struct A *)malloc(sizeof(struct A));	
	ptr_struct->a = 100;
	ptr_struct->b = 200;
	
	printf("%d \n",*ptr_int);
	printf("%hd \n",*ptr_shortint);
	printf("%d %d\n",ptr_struct->a, ptr_struct->b);
	
	free(ptr_shortint);
}
