/* Shows how to use brk sys call to deallocate memory
Version : 1.0
Author : Team -C
*/

#include <unistd.h>
#include <stdio.h>

void *current_add,*old_break;
int main()
{
	int ret,size;
	int *p;
	
	current_add = sbrk(0);	/* Fetch the current break point */
	old_break = current_add;

	getchar();
	
	size = sizeof(int);
	ret = brk(current_add+size);	/* Set new break point */
	if(ret)
		perror("brk :");
	
	getchar();
	* (int *)current_add = 10;
	printf("%d\n", * (int *)current_add);

	* (short int *)current_add = 20;
	* ((short int *)current_add+1) = 30;
	
        printf("%hd\n", * (short int *)current_add);
        printf("%hd\n", * ((short int *)current_add+1));
	
	ret = brk(old_break);	/* Set new break point */
	if(ret)
		perror("brk :");
	getchar();

}
