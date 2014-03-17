/* Shows how to use brk sys call to allocate memory for an integer
Version : 1.0
Author : Team -C
*/

#include <unistd.h>
#include <stdio.h>

void *current_add,*old_break,*new_break;
void main()
{
	int ret,size;
	int *p;
	
	current_add = sbrk(0);	/* Fetch the current break point */
	old_break = current_add;
	printf("current program break : %p\n",current_add);
	getchar();

	size = sizeof(int);
	
	new_break = current_add+size; 	
	ret = brk(new_break);	/* Set new break point */
	if(ret)
		perror("brk :");
	
	current_add = sbrk(0);
	printf("current program break : %p\n",current_add);
	getchar();
	
	printf("Heap Size = %d\n",current_add - old_break);	
	
}
