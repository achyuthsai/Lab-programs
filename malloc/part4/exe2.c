#include <stdio.h>
#include <malloc.h>
void my_malloc(void **p,size_t size)
{
	if(size<=0) {
		*p = NULL;
		return;
	}
	
	*p = malloc(size);
}

int main()
{
	int *ptr;

	my_malloc((void **)&ptr,0);
	if(ptr==NULL)
		printf("Failed to allocate memory\n");
		
}
