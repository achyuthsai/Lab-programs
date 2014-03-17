// Insertion Sort

#include<stdio.h>

void insertion_sort(int[],int);
void output(int[],int);

void main()
{
          int values[50],i,n;

          printf("\n How many elements you want to Enter ? : ");
          scanf("%d",&n);

          for(i=1;i<=n;i++)
          {
                   printf("\n Enter any number : ");
                   scanf("%d",&values[i]);
          }

          printf("\n List before sorting");
          output(values,n);

          insertion_sort(values,n);
          printf("\n List after sorting");
          output(values,n);
}

void output(int l[],int n)
{
          int i;

          for(i=1;i<=n;i++)
          {
                   printf("\n  %d",l[i]);
          }
}





void insertion_sort(int l[],int n)
{
          int temp,i,pointer;
          l[0]=0;
          for(i=1;i<=n;i++)
          {
            temp=l[i];
            pointer=i-1;

            while(temp<l[pointer])
             {
                   l[pointer+1]=l[pointer];
                   pointer--;
             }
             l[pointer+1]=temp;
          }
}
