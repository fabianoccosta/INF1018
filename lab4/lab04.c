#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

void dump (void *p, int n) {
  unsigned char *p1 = p;
  while (n--) {
    printf("%p - %02x\n", p1, *p1);
    p1++;
  }
}

int string2num (char *s, int base) {
  int a = 0;
  for (; *s; s++) {
    if(isdigit(*s))
      a = a*base + (*s - '0');
    else if((*s >= 'A') && (*s < (base-10+'A')))
      a = a*base + ((*s - 'A') + 10);
    else if((*s >= 'a') && (*s < (base-10+'a')))
      a = a*base + ((*s - 'a') + 10);
    else {
      printf("pane: numero invalido! \n");
      exit(1);
    }
  }
  return a;
}

int xbyte (unsigned int word, int bytenum) {
  int shifted = (int)word << ((3 - bytenum) * 8);
  return shifted >> 24;
}

int odd_ones(unsigned int x){
  x ^= x >> 16;
  x ^= x >> 8;
  x ^= x >> 4;
  x ^= x >> 2;
  x ^= x >> 1;
  return x & 1;
}

unsigned char switch_byte(unsigned char x){
  return (x >> 4) | (x << 4);
}

unsigned char rotate_left(unsigned char x, int n) {
  return (unsigned char)((x << n) | (x >> (8 - n)));
}

int main (void) {
  //1
  char c = 150;
  short s = -3;
  int i = -151;
  
  printf("--- Parte 1: Dumps ---\n");
  printf("dump de c:\n");
  dump(&c, sizeof(c));
  printf("dump de s:\n");
  dump(&s, sizeof(s));
  printf("dump de i:\n");
  dump(&i, sizeof(i));

  signed char sc = -1;
  unsigned int ui = sc;
  printf("ui = %u (%08x)\n", ui, ui);
  dump(&ui, sizeof(ui));

  //2
  printf("\n--- Parte 2: xbyte ---\n");
  unsigned int test_word = 0x11a032b5;
  printf("xbyte(0x11a032b5, 0) = %d\n", xbyte(test_word, 0));
  printf("xbyte(0x11a032b5, 1) = %d\n", xbyte(test_word, 1));
  printf("xbyte(0x11a032b5, 2) = %d\n", xbyte(test_word, 2));
  printf("xbyte(0x11a032b5, 3) = %d\n", xbyte(test_word, 3));

  //3
  printf("\n--- Parte 3: Comparações ---\n");
  int px = 0xffffffff;
  int py = 2;
  printf("Prog 1: x=%d, y=%d -> x < y? %s\n", px, py, (px < py) ? "sim" : "nao");

  unsigned int ux = 0xffffffff;
  unsigned int uy = 2;
  printf("Prog 2: x=%u, y=%u -> x < y? %s\n", ux, uy, (ux < uy) ? "sim" : "nao");

  printf("Prog 3: x(signed)=%d, y(unsigned)=%u -> x < y? %s\n", px, uy, (px < uy) ? "sim" : "nao");

 //4
  printf("\n--- Parte 4: Manipulação de Bits ---\n");
  unsigned int mx = 0x87654321;
  unsigned int my = mx & 0x000000FF;
  unsigned int mz = mx | 0xFF000000;
  
  printf("y = %08x\n", my);
  printf("z = %08x\n", mz);
  printf("odd_ones(y) = %d\n", odd_ones(my));

  unsigned char val1 = 0xAB;
  printf("switch_byte(%02x) = %02x\n", val1, switch_byte(val1));

  unsigned char val2 = 0x61;
  printf("rotate_left(%02x, 1) = %02x\n", val2, rotate_left(val2, 1));
  printf("rotate_left(%02x, 2) = %02x\n", val2, rotate_left(val2, 2));
  printf("rotate_left(%02x, 7) = %02x\n", val2, rotate_left(val2, 7));

  return 0;
}