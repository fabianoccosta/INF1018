#include <stdio.h>
#include <stdlib.h>
#include "gera_codigo.h"

int main(int argc, char *argv[]) {
  unsigned char code[500];
  funcp funcLBS;

  FILE *fp = fopen(argv[1], "r");      /* abre o txt passado no terminal */
  if (fp == NULL) { printf("nao abriu %s\n", argv[1]); return 1; }

  gera_codigo(fp, code, &funcLBS);     /* gera os bytes */
  fclose(fp);

  int res = funcLBS(atoi(argv[2]));    /* chama a função gerada */
  printf("resultado = %d\n", res);
  return 0;
}