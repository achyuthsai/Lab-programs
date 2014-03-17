#include<stdio.h>

int main()
{
    int arr[5], i=0;

	printf("%d %d %d %d %d\n",i, i++,  ++i, ++i, ++i);
	printf("%d\n", i);
/*    while(i<5)
        arr[i] = ++i;

    for(i=0; i<5; i++)
        printf("%d, ", arr[i]);
*/
    return 0;
}
