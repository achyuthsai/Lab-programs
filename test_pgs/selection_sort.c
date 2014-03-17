// Selection sort

#include<stdio.h>
void selection_sort(int[],int);
void output(int [],int);

void main()
{
          int values[50],i,n;

          printf("How Many Elements You Want To Enter? : ");
          scanf("%d",&n);

          for(i=0;i<n;i++)
          {
                   printf("\n Enter any number : ");
                   scanf("%d",&values[i]);
          }

          printf("\n List before sorting");
          output(values,n);

          selection_sort(values,n);

          printf("\n List after sorting");
          output(values,n);
}

void output(int values[],int n)
{
          int i;
          for(i=0;i<n;i++)
          {
                   printf("\n  %d",values[i]);
          }
}




void selection_sort(int a[],int n)
{
          int temp,i,j;

          for(i=0;i<n;i++)
          {
                   for(j=i+1;j<n;j++)
                   {
                             if(a[i]>a[j])
                             {
                                      temp=a[i];
                                      a[i]=a[j];
                                      a[j]=temp;
                             }
                   }
          }
}

