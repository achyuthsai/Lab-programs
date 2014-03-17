#include<stdio.h>

int main()
{

	int a = 2; 
	
	if(a==2) {

		a = ~a+(2<<1);
		printf("%d\n", a);

	}
	return 0;
}

