#include <stdio.h>

int t = 10;  

main(){

    int x = 0; 
    void funct1();
    funct1();            
    printf("After first call \n");
    funct1();            
    printf("After second call \n");
    funct1();            
    printf("After third call \n");

}
void funct1()
{
    static int y = 0;  
    int z = 10;             
    printf("value of y %d z %d",y,z);
    y=y+10;
}
