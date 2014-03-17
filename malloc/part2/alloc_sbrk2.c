/* Shows how to use brk sys call to allocate memory of 4
Version : 1.0
Author : Team -C
*/

#include <unistd.h>
#include <stdio.h>

void main()
{
	int ret,size=4;
	int *current_add;
	short int *p1,*p2;
	
	current_add = (int *)sbrk(size);	/* Allocating 4 bytes of memory */	

	*current_add = 10;
	printf("%d\n", *current_add);

	p1 = current_add;
	p2 = p1+1;

	*p1 = 20;
	*p2 = 30;
	
        printf("%d\n", *p1);
        printf("%d\n", *p2);
	
	sbrk(-size);	/* Deallocating memory */	
	
}
