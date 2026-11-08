/* Fabiano Carvalho Costa Júnior 2520144 3WA */
/* Gabriel Lourenço De Mello Vital 2520196 3WA*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gera_codigo.h"

/* Dicionario: assembly -> codigo de maquina (bytes obtidos com gcc -c + objdump -d)

   push %rbp              55
   mov  %rsp,%rbp         48 89 e5
   sub  $32,%rsp          48 83 ec 20
   mov  %edi,-24(%rbp)    89 7d e8         (salva p0 na pilha)
   mov  D(%rbp),%eax      8b 45 D          D = deslocamento da variavel
   mov  D(%rbp),%ecx      8b 4d D
   mov  D(%rbp),%edi      8b 7d D
   mov  $K,%eax           b8 K K K K       K = constante (4 bytes, little-endian)
   mov  $K,%ecx           b9 K K K K
   mov  $K,%edi           bf K K K K
   mov  %eax,D(%rbp)      89 45 D
   add  %ecx,%eax         01 c8
   sub  %ecx,%eax         29 c8
   imul %ecx,%eax         0f af c1
   cmp  $0,%eax           83 f8 00
   jne  +X                75 X             X = bytes pulados
   call rel32             e8 R R R R       R = distancia ate a funcao chamada
   leave                  c9
   ret                    c3

   Registro de ativacao (igual em todas as funcoes):
   v0 -4(%rbp) | v1 -8 | v2 -12 | v3 -16 | v4 -20 | p0 -24
   Sao 24 bytes, arredondados para 32 para manter a pilha alinhada em 16.
*/

/* Constante obrigatoria: a constante CEB respeita uma consistencia para a chamada da funcao gera_codigo.c */
const char CEB[] = "CEB";

/* push %rbp / mov %rsp,%rbp / sub $32,%rsp / mov %edi,-24(%rbp) */
unsigned char prologo[] = {0x55, 0x48, 0x89, 0xe5,
                           0x48, 0x83, 0xec, 0x20,
                           0x89, 0x7d, 0xe8};

/* leave / ret */
unsigned char epilogo[] = {0xc9, 0xc3};


/* Coloca o valor de um operando ($K, p0 ou vN) num registrador.
   reg: 'a' = %eax, 'c' = %ecx, 'd' = %edi
   Devolve a nova posicao do cursor. */
int valorOperandoEmReg(unsigned char code[], int idx, char *operando, char reg) {
    if (operando[0] == '$') {                          /* constante: mov $K, %reg */
        int valor;
        sscanf(operando, "$%d", &valor);
        if (reg == 'a') {
            code[idx] = 0xb8;
        }
        else if (reg == 'c') {
            code[idx] = 0xb9;
        }
        else {
            code[idx] = 0xbf;
        }
        idx++;
        code[idx] = valor & 0xff;          idx++;      /* 4 bytes, do menos */
        code[idx] = (valor >> 8) & 0xff;   idx++;      /* para o mais       */
        code[idx] = (valor >> 16) & 0xff;  idx++;      /* significativo     */
        code[idx] = (valor >> 24) & 0xff;  idx++;
    }
    else {                                             /* p0 ou vN: mov D(%rbp), %reg */
        unsigned char D;
        if (operando[0] == 'p') {
            D = 0xe8;                                  /* p0 mora em -24 */
        }
        else {
            int num;
            sscanf(operando, "v%d", &num);
            D = (unsigned char)(-4 * (num + 1));       /* vN mora em -4*(N+1) */
        }
        code[idx] = 0x8b;  idx++;
        if (reg == 'a') {
            code[idx] = 0x45;
        }
        else if (reg == 'c') {
            code[idx] = 0x4d;
        }
        else {
            code[idx] = 0x7d;
        }
        idx++;
        code[idx] = D;     idx++;
    }
    return idx;
}


void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    char linha[256];
    int idx_codigo = 0;          /* cursor: proxima posicao livre em code[] */
    int inicio_funcoes[50];      /* onde cada funcao comeca em code[] (no maximo 50 linhas) */
    int quantidade_funcoes = 0;  /* quantas funcoes ja foram lidas */

    *entry = NULL;

    while (fscanf(f, " %255[^\n]", linha) != EOF) {

        /* function: anota o inicio e escreve o prologo */
        if (strncmp(linha, "function", 8) == 0) {
            inicio_funcoes[quantidade_funcoes] = idx_codigo;
            quantidade_funcoes++;
            for (int i = 0; i < (int)sizeof(prologo); i++) {
                code[idx_codigo] = prologo[i];
                idx_codigo++;
            }
        }

        /* end: nada a fazer, o epilogo ja e escrito por cada ret/zret */
        else if (strncmp(linha, "end", 3) == 0) {
        }

        /* vD = call N X */
        else if (linha[0] == 'v' && strstr(linha, "call") != NULL) {
            int destino, n;
            char op[20];
            sscanf(linha, "v%d = call %d %s", &destino, &n, op);

            /* argumento -> %edi */
            idx_codigo = valorOperandoEmReg(code, idx_codigo, op, 'd');

            /* call: distancia = inicio da funcao n - fim desta instrucao */
            code[idx_codigo] = 0xe8;  idx_codigo++;
            int desl = inicio_funcoes[n] - (idx_codigo + 4);
            code[idx_codigo] = desl & 0xff;          idx_codigo++;
            code[idx_codigo] = (desl >> 8) & 0xff;   idx_codigo++;
            code[idx_codigo] = (desl >> 16) & 0xff;  idx_codigo++;
            code[idx_codigo] = (desl >> 24) & 0xff;  idx_codigo++;

            /* resultado (%eax) -> vD */
            code[idx_codigo] = 0x89;  idx_codigo++;
            code[idx_codigo] = 0x45;  idx_codigo++;
            code[idx_codigo] = (unsigned char)(-4 * (destino + 1));  idx_codigo++;
        }

        /* vD = X op Y */
        else if (linha[0] == 'v') {
            int destino;
            char op1[20], op2[20], op;
            sscanf(linha, "v%d = %s %c %s", &destino, op1, &op, op2);

            /* X -> %eax, Y -> %ecx */
            idx_codigo = valorOperandoEmReg(code, idx_codigo, op1, 'a');
            idx_codigo = valorOperandoEmReg(code, idx_codigo, op2, 'c');

            /* a conta (resultado fica em %eax) */
            if (op == '+') {                            /* add %ecx, %eax */
                code[idx_codigo] = 0x01;  idx_codigo++;
                code[idx_codigo] = 0xc8;  idx_codigo++;
            }
            else if (op == '-') {                       /* sub %ecx, %eax */
                code[idx_codigo] = 0x29;  idx_codigo++;
                code[idx_codigo] = 0xc8;  idx_codigo++;
            }
            else if (op == '*') {                       /* imul %ecx, %eax */
                code[idx_codigo] = 0x0f;  idx_codigo++;
                code[idx_codigo] = 0xaf;  idx_codigo++;
                code[idx_codigo] = 0xc1;  idx_codigo++;
            }

            /* %eax -> vD */
            code[idx_codigo] = 0x89;  idx_codigo++;     /* mov %eax, D(%rbp) */
            code[idx_codigo] = 0x45;  idx_codigo++;
            code[idx_codigo] = (unsigned char)(-4 * (destino + 1));  idx_codigo++;
        }

        /* zret X Y: se X == 0, retorna Y */
        else if (strncmp(linha, "zret", 4) == 0) {
            char op1[20], op2[20];
            sscanf(linha, "zret %s %s", op1, op2);

            idx_codigo = valorOperandoEmReg(code, idx_codigo, op1, 'a');  /* X -> %eax */

            code[idx_codigo] = 0x83;  idx_codigo++;     /* cmp $0, %eax */
            code[idx_codigo] = 0xf8;  idx_codigo++;
            code[idx_codigo] = 0x00;  idx_codigo++;

            code[idx_codigo] = 0x75;  idx_codigo++;     /* jne ... */
            int pos_salto = idx_codigo;                 /* anota onde fica o tamanho do pulo */
            code[idx_codigo] = 0x00;  idx_codigo++;     /* preenchido mais abaixo */

            idx_codigo = valorOperandoEmReg(code, idx_codigo, op2, 'a');  /* Y -> %eax */
            code[idx_codigo] = epilogo[0];  idx_codigo++;                 /* leave */
            code[idx_codigo] = epilogo[1];  idx_codigo++;                 /* ret   */

            code[pos_salto] = idx_codigo - (pos_salto + 1);               /* tamanho do pulo */
        }

        /* ret X */
        else if (strncmp(linha, "ret", 3) == 0) {
            char op[20];
            sscanf(linha, "ret %s", op);
            idx_codigo = valorOperandoEmReg(code, idx_codigo, op, 'a');   /* X -> %eax */
            code[idx_codigo] = epilogo[0];  idx_codigo++;                 /* leave */
            code[idx_codigo] = epilogo[1];  idx_codigo++;                 /* ret   */
        }
    }

    /* a funcao chamada pela main e a ultima do arquivo */
    if (quantidade_funcoes > 0)
        *entry = (funcp) &code[inicio_funcoes[quantidade_funcoes - 1]];
}