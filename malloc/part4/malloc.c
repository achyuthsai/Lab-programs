#include<stdio.h>
#include<malloc.h>


void test(int size)
{

	int *p;
	int i = 0;
	p = malloc(size);
	getchar();
	printf("\n %p \n", p);
	free(p);
	getchar();
}

int main()
{
	test(100);
	return 0;
}
