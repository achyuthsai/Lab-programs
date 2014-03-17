// quick sort

#include<stdio.h>

void quick_short(int[],int,int);
void output(int[],int);

void main()
{
  int a[50],i,n;

  printf("\n\nHow many elements you want to enter ? : ");
  scanf("%d",&n);

  for(i=0;i<n;i++)
  {
    printf("\n Enter any number : ");
    scanf("%d",&a[i]);
  }

  printf("\n list before sorting");
  output(a,n);

  quick_short(a,0,n-1);

  printf("\nList after sorting");
  output(a,n);
}

void output(int a[],int n)
{
  int i;
  for(i=0;i<n;i++)
  {
    printf("\n  %d",a[i]);
  }

}


void quick_short(int a[],int first, int last)
{
  int temp, high, low, pivot, i;

  low=first;
  high=last;
  pivot=a[(first+last)/2];

  do
  {

   while(a[low]<pivot)
   {
   low++;
   }
   while(a[high]>pivot)
   {
    high--;
   }
   if(low<=high)
    {
      temp=a[low];
      a[low]=a[high];
      a[high]=temp;
      low++;
      high--;
    }
  }while(low<=high);

 /* if(first<high)
  quick_short(a,first,high);

  if(low<last)
  quick_short(a,low,last);
*/
}
