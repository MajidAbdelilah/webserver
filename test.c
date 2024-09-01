#include <stdio.h>

int main (void)
{
    int a;
    int b;
    int c;
    
    a = 0;
    while(a <= 7){
        b = a + 1;
        while(b <= 8)
        {
            c = b + 1;
            while (c <= 9)
            {
                printf("%d%d%d, ", a, b, c);
                c++;
            }
            b++;
        }
        a++;
    }
}