/* Shows how to use brk sys call to allocate memory of 4
Version : 1.0
Author : Team -C
*/

#include <unistd.h>
#include <stdio.h>

void *current_add;
void main()
{
	int ret,size;
	int *p;
	
	current_add = sbrk(0);	/* Fetch the current break point */

	size = sizeof(int);
	
	ret = brk(current_add+size);	/* Set new break point */
	if(ret)
		perror("brk :");
	
	* (int *)current_add = 10;
	printf("%d\n", * (int *)current_add);

	* (short int *)current_add = 20;
	* ((short int *)current_add+1) = 30;
	
        printf("%d\n", * (short int *)current_add);
        printf("%d\n", * ((short int *)current_add+1));
	
}
