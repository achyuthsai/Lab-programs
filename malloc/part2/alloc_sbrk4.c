/* Shows how to use brk sys call to allocate memory of int, short int,struct A
Version : 1.0
Author : Team -C
*/

#include <unistd.h>
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
	
	ptr_int = (int *) sbrk(sizeof(int));		
	*ptr_int = 10;
	
	ptr_shortint = (short int *) sbrk(sizeof(short int));
	*ptr_shortint = 20;

	ptr_struct = (struct A *)sbrk(sizeof(struct A));	
	ptr_struct->a = 100;
	ptr_struct->b = 200;
	
	printf("%d \n",*ptr_int);
	printf("%hd \n",*ptr_shortint);
	printf("%d %d\n",ptr_struct->a, ptr_struct->b);

	sbrk(-sizeof(short int));
	
}
