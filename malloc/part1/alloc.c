#include <stdio.h>
#include <string.h>
#include <alloca.h>

void dup(int *p,int size)
{
	int *temp,i;

	temp = alloca(size*sizeof(int));

	memcpy(temp,p,size);
	
	for(i=0;i<size;i++)
		printf(" %d",temp[i]);
	
}

int main()
{
	int a[5] = {1,2,3,4,5},i;

	dup(a,5);
	for(i=0;i<5;i++)
		printf(" %d",a[i]);

	printf("\n");
	
	
}
