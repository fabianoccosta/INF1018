#include <stdio.h>

void dump (void *p, int n) {
  unsigned char *p1 = p;
  int i;
  for (i=0; i<n; i++) {
    printf("%02x ", p1[i]);
  }
  printf("\n");
}

struct X {
  int a;
  short b;
  int c;
};

struct X1 {
  char c1;
  int i;
  char c2;
};

struct X2 {
  long l;
  char c;
};

struct X3 {
  int i;
  char c1;
  char c2;
};

struct X4 {
  struct X2 x;
  char c;
};

struct X5 {
  char c1;
  char c2;
  char c3;
};

struct X6 {
  short s1;
  int i;
  char c[3];
  short s2;
};

union U1 {
  int i;
  char c[5];
};

union U2 {
  short s;
  char c[5];
};

int main (void) {
  int i, j;
  short a[2][3];
  int b[2];

  for (i=0; i<2; i++) {
    b[i] = i;
    for (j=0; j<3; j++)
      a[i][j] = 3*i+j;
  }

  printf ("b (tam %lu):\n", sizeof(b));
  dump (b, sizeof(b));
  printf ("a (tam %lu):\n", sizeof(a));
  dump (a, sizeof(a));

  struct X x = {0xa1a2a3a4, 0xb1b2, 0xc1c2c3c4};
  printf ("X (tam %lu):\n", sizeof(x));
  dump (&x, sizeof(x));

  struct X1 x1 = {0x11, 0x22334455, 0x66};
  printf ("X1 (tam %lu):\n", sizeof(x1));
  dump (&x1, sizeof(x1));

  struct X2 x2 = {0x1122334455667788L, 0x99};
  printf ("X2 (tam %lu):\n", sizeof(x2));
  dump (&x2, sizeof(x2));

  struct X3 x3 = {0x11223344, 0x55, 0x66};
  printf ("X3 (tam %lu):\n", sizeof(x3));
  dump (&x3, sizeof(x3));

  struct X4 x4 = {{0x1122334455667788L, 0x99}, 0xaa};
  printf ("X4 (tam %lu):\n", sizeof(x4));
  dump (&x4, sizeof(x4));

  struct X5 x5 = {0x11, 0x22, 0x33};
  printf ("X5 (tam %lu):\n", sizeof(x5));
  dump (&x5, sizeof(x5));

  struct X6 x6 = {0x1122, 0x33445566, {0x71, 0x72, 0x73}, 0x8182};
  printf ("X6 (tam %lu):\n", sizeof(x6));
  dump (&x6, sizeof(x6));

  union U1 u1 = {.i = 0x11223344};
  printf ("U1 (tam %lu):\n", sizeof(u1));
  dump (&u1, sizeof(u1));

  union U2 u2 = {.s = 0x1122};
  printf ("U2 (tam %lu):\n", sizeof(u2));
  dump (&u2, sizeof(u2));

  return 0;
}