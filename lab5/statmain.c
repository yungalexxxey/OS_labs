#include <stdio.h>
#include "mylib.h"
int main()
{
    int choose = -1;
    float A, B, e;
    int a, b;
    while (choose != 3)
    {
        printf("1-sinIntegral\n2-GCF\n3-Exit\n");
        scanf("%d", &choose);
        switch (choose)
        {
        case 1:
        {
            printf("Write A,B,e: ");
            scanf("%f", &A);
            scanf("%f", &B);
            scanf("%f", &e);
            printf("sinIntegral:%f\n", sinIntegral(A, B, e));
            continue;
        }
        case 2:
        {
            printf("Write A and B: ");
            scanf("%d", &a);
            scanf("%d", &b);
            printf("GCF is %d\n", (*GCF)(a, b));
            continue;
        }
        case 3:
        {
            printf("The end\n");
            continue;
        }
        default:
        {
            printf("Wrong num\n");
            continue;
        }
        }
    }
    return 0;
}