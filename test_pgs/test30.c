#include<stdio.h>

int main()
{
	long long a[3];
	int b[3][3];

	printf("a:%d\n", sizeof(a));
	printf("b:%d\n", sizeof(*b));
}
