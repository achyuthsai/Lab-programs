// bubble sort

#include<stdio.h>

void bubble_sort(int[],int);
void output(int [],int);

void main()
{
     int values[50],n,i,j,temp;

     printf("How Many Elements You Want To Enter ?  :");
     scanf("%d",&n);

     for(i=0;i<n;i++)
       {
            printf("\n Enter any number : ");
            scanf("%d",&values[i]);
       }

       printf("\n List before sorting");
       output(values,n);

       bubble_sort(values,n);

       printf("\n list after sorting ");
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


void bubble_sort(int a[],int size)
 {
      int i,j,temp;

      for(i=0;i<size;i++)
       {
            for(j=0;j<size-i-1;j++)
             {
                   if(a[j]>a[j+1])
                   {
                             temp=a[j];
                             a[j]=a[j+1];
                             a[j+1]=temp;
                   }
             }
          }
 }
