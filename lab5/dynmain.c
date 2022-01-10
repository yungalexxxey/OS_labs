#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

void *my_lib;
char *name_first = "my_lib1.so";
char *name_sec = "my_lib2.so";
float (*sinIntegral)(float A, float B, float e);
int (*GCF)(int A, int B);
int curr = 1;

void change()
{
    dlclose(my_lib);
    if (curr == 1)
    {
        my_lib = dlopen(name_sec, RTLD_LAZY);
        curr++;
        sinIntegral = dlsym(my_lib, "sinIntegral");
        GCF = dlsym(my_lib, "GCF");
        printf("second lib was chosen\n");
    }
    else
    {
        my_lib = dlopen(name_first, RTLD_LAZY);
        curr--;
        sinIntegral = dlsym(my_lib, "sinIntegral");
        GCF = dlsym(my_lib, "GCF");
        printf("first lib was chosen\n");
    }
}

int main()
{
    float A, B, e;
    int a, b;
    int choose = -1;

    my_lib = dlopen("my_lib1.so", RTLD_LAZY);
    if (!my_lib)
    {
        fprintf(stderr, "the big error\n", dlerror());
        return -1;
    }

    sinIntegral = dlsym(my_lib, "sinIntegral");
    GCF = dlsym(my_lib, "GCF");
    while (choose != 3)
    {
        printf("0-switch contract\n1-sinIntegral\n2-GCF\n3-Exit\n");
        scanf("%d", &choose);
        switch (choose)
        {
        case 0:
        {
            change();
            continue;
        }
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