#include <stdio.h>
#include "gera_codigo.h"

/* uso: ./teste arquivo.lbs argumento */
int main(int argc, char *argv[]) {
  FILE *fp;
  unsigned char code[1024];
  funcp funcLBS;
  int res, arg = 0;

  if (argc < 2) {
    printf("uso: %s arquivo.lbs [argumento]\n", argv[0]);
    return 1;
  }
  if (argc > 2) sscanf(argv[2], "%d", &arg);

  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    printf("Erro ao abrir %s\n", argv[1]);
    return 1;
  }
  gera_codigo(fp, code, &funcLBS);
  fclose(fp);
  if (funcLBS == NULL) {
    printf("Erro na geracao\n");
    return 1;
  }
  res = (*funcLBS)(arg);
  printf("%s(%d) = %d\n", argv[1], arg, res);
  return 0;
}
