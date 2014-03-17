#include<stdio.h>

int main ()
{
	int a,b;

	a = sizeof(signed) + sizeof(unsigned);
	b = sizeof(const) + sizeof(volatile);

	printf("%d\n", a);
	printf("%d\n", b);

	return 0;
}

