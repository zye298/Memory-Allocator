#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 8

int main() {


    signed int b = 16;
    signed int x = 0;
    for (signed int i = 0; i < (signed int)N; i++) 
        x++;
    b = (signed int)b >> (signed int)x;

    printf("the num is %d\n", b);
    
    return 0;
}