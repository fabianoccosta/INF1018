#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#define PAGE_START(P) ((intptr_t)(P)&~(pagesize-1))
#define PAGE_END(P) (((intptr_t)(P)+pagesize-1)&~(pagesize-1))

int execpage(void *ptr, size_t len) {
  int ret;
  const long pagesize = sysconf(_SC_PAGE_SIZE);
  if (pagesize == -1)
    return -1;
  ret = mprotect((void *)PAGE_START(ptr),
                 PAGE_END((intptr_t)ptr + len) - PAGE_START(ptr),
                 PROT_READ | PROT_WRITE | PROT_EXEC);
  if (ret == -1)
    return -1;
  return 0;
}

#undef PAGE_START
#undef PAGE_END

typedef int (*funcp)(int x);

int add(int x) {
  return x + 1;
}

int main(void) {
  int i;

  unsigned char codigo1[] = {0x83, 0xc7, 0x01, 0x89, 0xf8, 0xc3};
  execpage(codigo1, sizeof codigo1);
  funcp f1 = (funcp)codigo1;
  i = (*f1)(10);
  printf("Parte 1 (x+1):       foo(10) = %d\n", i);
                           */
  static unsigned char codigo2[] = {0x55, 0x48, 0x89, 0xe5,
                             0xe8, 0x00, 0x00, 0x00, 0x00,
                             0xc9, 0xc3};

  int desl = (int)((long)add - (long)(codigo2 + 9));
  codigo2[5] = desl & 0xff;          
  codigo2[6] = (desl >> 8) & 0xff;
  codigo2[7] = (desl >> 16) & 0xff;
  codigo2[8] = (desl >> 24) & 0xff;
  execpage(codigo2, sizeof codigo2);
  funcp f2 = (funcp)codigo2;
  i = (*f2)(10);
  printf("Parte 2 (call add):  foo(10) = %d\n", i);

  static unsigned char codigo3[] = {0xe9, 0x00, 0x00, 0x00, 0x00};

  desl = (int)((long)add - (long)(codigo3 + 5));
  codigo3[1] = desl & 0xff;
  codigo3[2] = (desl >> 8) & 0xff;
  codigo3[3] = (desl >> 16) & 0xff;
  codigo3[4] = (desl >> 24) & 0xff;
  execpage(codigo3, sizeof codigo3);
  funcp f3 = (funcp)codigo3;
  i = (*f3)(10);
  printf("Parte 3 (jmp add):   foo(10) = %d\n", i);

  return 0;
}
