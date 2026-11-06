#include <stdio.h>

#define getsig(x)        ((x)>>31 & 1)
#define getexp(x)        ((x)>>23 & 0xff)
#define getfrac(x)       ((x) & 0x7fffff)
#define makefloat(s,e,f) ((s & 1)<<31 | (((e) & 0xff) << 23) | ((f) & 0x7fffff))

typedef union {
  float f;
  unsigned int i;
} U;

/* Multiplica um float por 2 operando apenas sobre os bits.
   x = (-1)^s * M * 2^E  =>  2x = (-1)^s * M * 2^(E+1): basta somar 1 a exp. */
float float2(float f) {
  U u;
  unsigned int s, e, frac;

  u.f = f;
  s = getsig(u.i);
  e = getexp(u.i);
  frac = getfrac(u.i);

  if (e == 0xff)                 /* infinito ou NaN: permanece igual */
    return f;

  if (e == 0) {                  /* zero ou denormalizado: E = -126 fixo, M = 0.frac */
    frac <<= 1;                  /* dobrar M equivale a deslocar frac 1 bit */
    if (frac & 0x800000) {       /* virou normalizado (M = 1.xxx): exp passa a 1 */
      e = 1;
      frac &= 0x7fffff;
    }
  } else {                       /* normalizado: E aumenta em 1 */
    e++;
    if (e == 0xff)               /* estouro: vira infinito */
      frac = 0;
  }

  u.i = makefloat(s, e, frac);
  return u.f;
}

/* Converte int para float sem usar a conversão do C. */
float int2float(int i) {
  U u;
  unsigned int s, abs, e, frac, resto, meio;
  int pos;

  if (i == 0)                    /* caso especial: representação toda zero */
    return 0.0f;

  s = (i < 0);
  abs = s ? -(unsigned int)i : (unsigned int)i;   /* ok também para INT_MIN */

  /* pos = posição do bit 1 mais significativo: abs = 1.xxx * 2^pos, logo E = pos */
  pos = 31;
  while (!((abs >> pos) & 1))
    pos--;

  if (pos <= 23) {
    /* cabe nos 23 bits de frac: alinha o bit após o "1" implícito na posição 22 */
    frac = (abs << (23 - pos)) & 0x7fffff;
    e = pos + 127;
  } else {
    /* perde (pos-23) bits: arredonda para o mais próximo (empate -> par) */
    int perdidos = pos - 23;
    frac  = (abs >> perdidos) & 0x7fffff;
    resto = abs & ((1u << perdidos) - 1);
    meio  = 1u << (perdidos - 1);
    e = pos + 127;
    if (resto > meio || (resto == meio && (frac & 1))) {
      frac++;
      if (frac == 0x800000) {    /* vai-um propagou: mantissa vira 10.0 -> 1.0, E+1 */
        frac = 0;
        e++;
      }
    }
  }

  u.i = makefloat(s, e, frac);
  return u.f;
}
