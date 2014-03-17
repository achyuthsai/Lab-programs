#include<stdio.h>
#include<string.h>

int main()
{
    int i, n;
    char *x="Alice";
    n = strlen(x);
    *x = x[n-1];
    for(i=0; i<=n; i++)
    {
        printf("%s ", x);
        x++;
    }
    printf("\n", x);
    return 0;
}
