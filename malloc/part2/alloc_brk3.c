/* Shows how to use brk sys call to deallocate memory
Version : 1.0
Author : Team -C
*/

#include <unistd.h>
#include <stdio.h>


struct msg {
	char str[25];
	int len;
};

int main()
{
	struct mgs  *current_add;
	int ret,size;
	int i;
	
	current_add = sbrk(0);	/* Fetch the current break point */
	old_break = current_add;

	size = sizeof(struct mgs) * 3;
	ret = brk(current_add+size);	/* Set new break point */
	if(ret)
		perror("brk :");
	
	strcpy(current_add[0].str,"brk API") ;
	strcpy(current_add[1].str,"sbrk API") ;
	strcpy(current_add[0].str,"unix interfaces") ;
	
	for(i=0;i<3;i++)
		current_add[i].len = strlen(current_add[i].str);

	for(i=0;i<3;i++)
        	printf("%s %d\n", current_add[i].str,current_add[i].len);
	
}
