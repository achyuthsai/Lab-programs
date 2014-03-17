#include<stdio.h>
struct marks{
  int p:3;
  int c:3;
  int m:2;
};
void main(){
  struct marks s={2,-6,5};
  printf("%d %d %d",s.p,s.c,s.m); 
}
