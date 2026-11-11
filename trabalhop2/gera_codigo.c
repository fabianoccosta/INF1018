/*INF1018 - Software Básico - 2026.2

Trabalho G2: Gerador de código para a linguagem LBS.

Integrantes:

  Fabiano Carvalho Costa Júnior - 2520144 - 3WA

  Gabriel Lourenço - 2520196 - 3WA*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gera_codigo.h"

// dicionario dos codigos em assembly porque se não nem lembro:
// push %rbp            55
// mov %rsp,%rbp        48 89 e5
// sub $32,%rsp         48 83 ec 20 (sempre 32)
// mov %edi,-24(%rbp)   89 7d e8 (salva p0)
// mov D(%rbp),%eax     8b 45 D
// mov D(%rbp),%ecx     8b 4d D
// mov D(%rbp),%edi     8b 7d D
// mov $K,%eax          b8 K K K K
// mov $K,%ecx          b9 K K K K
// mov $K,%edi          bf K K K K
// mov %eax,D(%rbp)     89 45 D
// add %ecx,%eax        01 c8
// sub %ecx,%eax        29 c8
// imul %ecx,%eax       0f af c1
// cmp $0,%eax          83 f8 00
// jne +X               75 X (X = bytes pulados)
// call rel32           e8 R R R R (R = deslocamento)
// leave / ret          c9 / c3

// prologo completo: push %rbp / mov %rsp,%rbp / sub $32,%rsp / mov %edi,-24(%rbp) Abrindo RA em 32bytes e salvando %edi na pilha
unsigned char prologo[] = {0x55, 0x48, 0x89, 0xe5, 0x48, 0x83, 0xec, 0x20, 0x89, 0x7d, 0xe8};
unsigned char epilogo[] = {0xc9, 0xc3};

// escreve um byte no vetor code e avanca o indice
static void byte(unsigned char code[], int *idx, unsigned char b) {
    code[*idx] = b;
    (*idx)++;
}

// escreve um inteiro de 4 bytes em little-endian
static void inteiro(unsigned char code[], int *idx, int valor) {
    byte(code, idx, valor & 0xff);
    byte(code, idx, (valor >> 8) & 0xff);
    byte(code, idx, (valor >> 16) & 0xff);
    byte(code, idx, (valor >> 24) & 0xff);
}

// deslocamento em relacao a %rbp: v0..v4 -> -4,-8,...,-20 ; p0 -> -24
static int deslocamento(char *op) {
    if (op[0] == 'p') return -24;
    return -4 * (atoi(op + 1) + 1);
}

// carrega o operando (vN, p0 ou $K) em um registrador: 0 = %eax, 1 = %ecx, 2 = %edi
static void carrega(unsigned char code[], int *idx, char *op, int reg) {
    if (op[0] == '$') {
        unsigned char opcode[] = {0xb8, 0xb9, 0xbf};
        byte(code, idx, opcode[reg]);                 // mov $K,%reg
        inteiro(code, idx, atoi(op + 1));
    } else {
        unsigned char modrm[] = {0x45, 0x4d, 0x7d};
        byte(code, idx, 0x8b);                        // mov D(%rbp),%reg
        byte(code, idx, modrm[reg]);
        byte(code, idx, (unsigned char)deslocamento(op));
    }
}

// leave ; ret
static void escreve_epilogo(unsigned char code[], int *idx) {
    byte(code, idx, epilogo[0]);  // leave
    byte(code, idx, epilogo[1]);  // ret
}

void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    char linha[256];
    int idx_codigo = 0; // controla onde escrever o proximo byte

    // array pra guardar onde cada funcao comeca (max 50 funcoes)
    // serve pra resolver os call e o ponteiro entry dps
    int inicio_funcoes[50];
    int qtd_funcoes = 0;

    *entry = NULL;

    while (fscanf(f, " %255[^\n]", linha) != EOF) {
        if (strncmp(linha, "function", 8) == 0) {
            // salva o endereco como inicio de uma nova funcao
            inicio_funcoes[qtd_funcoes] = idx_codigo;
            qtd_funcoes++;
            
            // joga o prologo
            for (int i = 0; i < (int)sizeof(prologo); i++) {
                code[idx_codigo] = prologo[i];
                idx_codigo++;
            }
        }
        else if (strncmp(linha, "end", 3) == 0) {
            // n faz nada pq o epilogo ja vai no ret/zret
        }
        else if (strncmp(linha, "zret", 4) == 0) {
            char a[64], b[64];
            sscanf(linha, "zret %63s %63s", a, b);

            carrega(code, &idx_codigo, a, 0);                 // mov a,%eax
            byte(code, &idx_codigo, 0x83);                    // cmp $0,%eax
            byte(code, &idx_codigo, 0xf8);
            byte(code, &idx_codigo, 0x00);
            byte(code, &idx_codigo, 0x75);                    // jne +X
            
            // X = tamanho do bloco: mov b,%eax (3 ou 5 bytes) + leave + ret
            byte(code, &idx_codigo, (b[0] == '$' ? 5 : 3) + 2);
            carrega(code, &idx_codigo, b, 0);                 // mov b,%eax
            escreve_epilogo(code, &idx_codigo);
        }
        else if (strncmp(linha, "ret", 3) == 0) {
            char a[64];
            sscanf(linha, "ret %63s", a);

            carrega(code, &idx_codigo, a, 0);                 // mov ___,%eax
            escreve_epilogo(code, &idx_codigo);               // leave ; ret
        }
        else {
            // atribuicao: vN = call F x   ou   vN = x op y
            int var, num;
            char a[64], b[64], op;

            if (sscanf(linha, "v%d = call %d %63s", &var, &num, a) == 3) {
                carrega(code, &idx_codigo, a, 2);             // mov x,%edi
                byte(code, &idx_codigo, 0xe8);                // call rel32
                inteiro(code, &idx_codigo, inicio_funcoes[num] - (idx_codigo + 4));
                byte(code, &idx_codigo, 0x89);                // mov %eax,D(%rbp)
                byte(code, &idx_codigo, 0x45);
                byte(code, &idx_codigo, (unsigned char)(-4 * (var + 1)));
            }
            else if (sscanf(linha, "v%d = %63s %c %63s", &var, a, &op, b) == 4) {
                carrega(code, &idx_codigo, a, 0);             // mov x,%eax
                carrega(code, &idx_codigo, b, 1);             // mov y,%ecx
                
                if (op == '+') {                              // add %ecx,%eax
                    byte(code, &idx_codigo, 0x01);
                    byte(code, &idx_codigo, 0xc8);
                } else if (op == '-') {                       // sub %ecx,%eax
                    byte(code, &idx_codigo, 0x29);
                    byte(code, &idx_codigo, 0xc8);
                } else {                                      // imul %ecx,%eax
                    byte(code, &idx_codigo, 0x0f);
                    byte(code, &idx_codigo, 0xaf);
                    byte(code, &idx_codigo, 0xc1);
                }
                
                byte(code, &idx_codigo, 0x89);                // mov %eax,D(%rbp)
                byte(code, &idx_codigo, 0x45);
                byte(code, &idx_codigo, (unsigned char)(-4 * (var + 1)));
            }
        }
    }

    // seta o entry pra apontar pra ultima funcao lida
    if (qtd_funcoes > 0)
        *entry = (funcp)(code + inicio_funcoes[qtd_funcoes - 1]);
}