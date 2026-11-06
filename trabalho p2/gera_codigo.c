#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*dicionario dos codigos em assembly pq se n nem lembro:

push %rbp	55	nada
mov %rsp,%rbp	48 89 e5	nada
sub $32,%rsp	48 83 ec 20	nada (sempre 32)
mov %edi,-24(%rbp) (salva p0)	89 7d e8	nada
mov D(%rbp),%eax	8b 45 D	D = offset
mov D(%rbp),%ecx	8b 4d D	D
mov D(%rbp),%edi	8b 7d D	D
mov $K,%eax	b8 K K K K	K (4 bytes, little-endian)
mov $K,%ecx	b9 K K K K	K
mov $K,%edi	bf K K K K	K
mov %eax,D(%rbp)	89 45 D	D
add %ecx,%eax	01 c8	nada
sub %ecx,%eax	29 c8	nada
imul %ecx,%eax	0f af c1	nada
cmp $0,%eax	83 f8 00	nada
jne +X	75 X	X = bytes pulados
call rel32	e8 R R R R	R = deslocamento
leave / ret	c9 / c3	nada
*/

/* Constante obrigatoria: a constante CEB respeita uma consistencia para a chamada da funcao gera_codigo.c */
const char CEB[] = "CEB"; 

typedef int (*funcp) (int x);

/* Coloca o valor de um operando ($K, p0 ou vN) num registrador.
   reg: 'a' = %eax, 'c' = %ecx
   Devolve a nova posicao do cursor. */
int valorOperandoEmReg(unsigned char code[], int idx, char *operando, char reg) {
    if (operando[0] == '$') {                          /* constante: mov $K, %reg */
        int valor;
        sscanf(operando, "$%d", &valor);
        if (reg == 'a'){
            code[idx] = 0xb8;
        }
        else {
            code[idx] = 0xb9;
        }           
        idx++;
        code[idx] = valor & 0xff;          idx++;
        code[idx] = (valor >> 8) & 0xff;   idx++;
        code[idx] = (valor >> 16) & 0xff;  idx++;
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
        if (reg == 'a'){
           code[idx] = 0x45;
        } 
        else{
           code[idx] = 0x4d; 
        } 

        idx++;
        code[idx] = D;     
        idx++;
    }
    return idx;
}

unsigned char prologo[] = {0x55, 0x48, 0x89, 0xe5,     /* push %rbp; mov %rsp,%rbp */
                           0x48, 0x83, 0xec, 0x20,     /* sub $32,%rsp            */
                           0x89, 0x7d, 0xe8};          /* mov %edi,-24(%rbp)      */
                           
unsigned char epilogo[] = {0xc9, 0xc3};

void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    char linha[256];
    int idx_codigo = 0; // Índice que controla onde escrever o próximo byte no vetor 'code'
    
    int inicio_funcoes[50]; //50 pq vai no maximo até 50 linhas, n que seja possivel 50 linhas com "function"
    int quantidade_funcoes = 0; // vai servir de indicie para colocar no inicio_funcoes

    while (fscanf(f, " %[^\n]", linha) != EOF) {
        if (strncmp(linha, "function", 8) == 0) {
            inicio_funcoes[quantidade_funcoes] = idx_codigo;   /* 1. anota onde esta função começa */
            quantidade_funcoes++;
            for (int i = 0; i < 11; i++) {          /* o for do prólogo mora AQUI */
                code[idx_codigo] = prologo[i];
                idx_codigo++;
            }
            // TODO: Salvar o endereço (idx_codigo) como o início de uma nova função
            // TODO: Inserir os bytes do prólogo do Assembly
            
        } 
        else if (strncmp(linha, "end", 3) == 0) {
            // TODO: Inserir os bytes do epílogo do Assembly
        }
        else if (linha[0] == 'v') {                     /* vD = X op Y */
            int destino;
            char op1[20], op2[20], op;
            sscanf(linha, "v%d = %s %c %s", &destino, op1, &op, op2);

            /* passo 1 e 2: X → %eax, Y → %ecx */
            idx_codigo = valorOperandoEmReg(code, idx_codigo, op1, 'a');
            idx_codigo = valorOperandoEmReg(code, idx_codigo, op2, 'c');

            /* passo 3: a conta (resultado fica em %eax) */
            if (op == '+') {                            /* add %ecx, %eax */
                code[idx_codigo] = 0x01;  idx_codigo++;
                code[idx_codigo] = 0xc8;  idx_codigo++;
            } else if (op == '-') {                     /* sub %ecx, %eax */
                code[idx_codigo] = 0x29;  idx_codigo++;
                code[idx_codigo] = 0xc8;  idx_codigo++;
            } else if (op == '*') {                     /* imul %ecx, %eax */
                code[idx_codigo] = 0x0f;  idx_codigo++;
                code[idx_codigo] = 0xaf;  idx_codigo++;
                code[idx_codigo] = 0xc1;  idx_codigo++;
            }

            /* passo 4: %eax → gaveta de vD */
            code[idx_codigo] = 0x89;  idx_codigo++;     /* mov %eax, D(%rbp) */
            code[idx_codigo] = 0x45;  idx_codigo++;
            code[idx_codigo] = (unsigned char)(-4 * (destino + 1));  idx_codigo++;
        }
       else if (strncmp(linha, "ret", 3) == 0) {
            char op[20];
            sscanf(linha, "ret %s", op);                     /* op = "$100", "p0" ou "v2" */
            idx_codigo = valorOperandoEmReg(code, idx_codigo, op, 'a'); /* valor → %eax */
            code[idx_codigo] = epilogo[0];  idx_codigo++;    /* leave */
            code[idx_codigo] = epilogo[1];  idx_codigo++;    /* ret   */
         }
    } 
    //falta zRet   
    *entry = (funcp) &code[inicio_funcoes[quantidade_funcoes - 1]];
}