#include<stdio.h>

int fun(char *s)
{
	static int x = 10;
	strlen(s);
	printf("%d\n", x);
	return x;
}

int main()
{
	int x;
	char *s = "string1";

	x = fun(s);

	x = x +10;
	x = fun(s);

	
	return 0;
}
