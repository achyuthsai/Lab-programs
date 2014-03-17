#include <stdio.h>

#define ARR_SIZE 5
#define ELE_SIZE sizeof(int)

int main()
{
	int *p,i;

	p = calloc(ARR_SIZE,ELE_SIZE);
	if(p==NULL) 
		exit(1);
		
	for(i=0;i<ARR_ELE;i++)
		p[i]=i+1;

	for(i=0;i<ARR_ELE;i++)
		printf("%d ",p[i]);

	printf("\n");	

	 
}
