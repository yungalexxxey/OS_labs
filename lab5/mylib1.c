#include <math.h>
#include "mylib.h"
float sinIntegral(float a,float b,float c){
    float step = c;
    int count = (int)((b - a) / step);

    float ans = 0;
    for (int i = 0; i <= count; i++) {
        ans += step * sinf(a + (float)(i - 1) * step);
    }
    return ans;
}

int GCF(int A,int B){
while(A!=B){
    if(A>B) A=A-B;
    else B=B-A;
}
return A;
}