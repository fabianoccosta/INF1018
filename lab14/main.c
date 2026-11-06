#include <stdio.h>
#include <math.h>

#define getsig(x)        ((x)>>31 & 1)
#define getexp(x)        ((x)>>23 & 0xff)
#define getfrac(x)       ((x) & 0x7fffff)
#define makefloat(s,e,f) ((s & 1)<<31 | (((e) & 0xff) << 23) | ((f) & 0x7fffff))

typedef union {
  float f;
  unsigned int i;
} U;

float int2float(int i);
float float2(float f);

int main() {
   int i;
   float f;

   printf("\n******** float2 ****************\n");
   f = 0.0f;
   printf(" %+10.4f -> %+10.4f\n", f, float2(f));
   f = 1.0f;
   printf(" %+10.4f -> %+10.4f\n", f, float2(f));
   f = -2.5f;
   printf(" %+10.4f -> %+10.4f\n", f, float2(f));
   f = 3.14159f;
   printf(" %+10.4f -> %+10.4f\n", f, float2(f));
   f = -123.456f;
   printf(" %+10.4f -> %+10.4f\n", f, float2(f));

   printf("\n******** int2float ****************\n");
   i = 0;
   printf(" %d -> %+10.4f\n", i, int2float(i));
   i = 1;
   printf(" %d -> %+10.4f\n", i, int2float(i));
   i = -1;
   printf(" %d -> %10.4f\n", i, int2float(i));
   i = 0x7fffffff;
   printf(" %d -> %+10.4f\n", i, int2float(i));
   i = -i;
   printf(" %d -> %+10.4f\n", i, int2float(i));
   i = 12345;
   printf(" %d -> %+10.4f\n", i, int2float(i));
   i = -12345;
   printf(" %d -> %+10.4f\n", i, int2float(i));
   return 0;
}
