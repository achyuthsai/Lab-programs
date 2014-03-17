#include <stdio.h>

#define ARR_SIZE 5
#define ELE_SIZE sizeof(int)
#define NEW_SIZE(s) sizeof(int)*s

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

	p = realloc(p,NEW_SIZE(10));
	
	for(i=5;i<ARR_ELE+5;i++)
		p[i] = i+1;

	for(i=0;i<ARR_ELE+5;i++)
		printf("%d ",p[i]);
		
	printf("\n");	

	 
}
