#include <math.h>
#include "mylib.h"
float sinIntegral(float a,float b,float c){
    int n = (int)((b - a) / c);

    float h = (b-a)/(float)n;

    float s = sinf(a)+sinf(b);

    for (int i = 1; i < n; i++)
        s += 2*sinf(a+(float)i*h);
    return h*s/2;
}

int GCF(int A,int B){
    int min;
    int res=0;
    if(A<B) min=A;
    else min=B;
    for(int i=1;i<=min;i++){
        if( A%i==0 && B%i==0 ) res=i;
    } 
    return res;
}