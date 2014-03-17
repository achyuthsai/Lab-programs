// Binary search
#include<stdio.h>

void bubble_sort(int[],int);
void binary_search(int,int,int[]);

void main()
{
     int a[50],n,no,i;

     printf("How Many Elements You Want To Enter? :   ");
     scanf("%d",&n);

     for(i=0;i<n;i++)
       {
            printf("\n Enter any number : ");
            scanf("%d",&a[i]);
       }
     // First sort the list in ascending order
     bubble_sort(a,n);

     printf("the sorted list is :->\n");

     for(i=0;i<n;i++)
       {
            printf("\n  %d",a[i]);
       }

       printf("\n\nEnter Element To Search : ");
       scanf("%d",&no);
       binary_search(no,n,a); // call binary search after sorting

     }

// bubble sort
void bubble_sort(int a[],int size)
 {
     int i,j,temp;
     for(i=0;i<size;i++)
       {
                for(j=0;j<size-1;j++)
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

// binary search
void binary_search(int e,int n,int l[])
{
          int high,low,mid,flag=0;
          high=n-1;
          low=0;
          mid=(low+high)/2;

          while(flag!=1 && low<=high)
          {
                   if(e<l[mid])
                   {
                             high=mid-1;
                   }
                   else if(e>l[mid])
                   {
                             low=mid+1;
                   }
                   else if(e==l[mid])
                   {
                             printf("\nSearch Is Successful");
                             printf("\nElement %d Found at location: %d",e,mid);
                             flag=1;
                   }  
		 mid=(low+high)/2;
          }
          if(flag==0)
          {
            printf("\n\nSearch Is Unsuccessful");
          }
}

