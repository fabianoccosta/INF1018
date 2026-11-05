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

unsigned char prologo[] = {0x55, 0x48, 0x89, 0xe5};
unsigned char epilogo[] = {0xc9, 0xc3};

void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    char linha[256];
    int idx_codigo = 0; // Índice que controla onde escrever o próximo byte no vetor 'code'
    
    // Você precisará de uma forma de rastrear onde cada função começa
    // para preencher o ponteiro entry no final e para lidar com chamadas (call).
    int inicio_funcoes[50]; //50 pq vai no maximo até 50 linhas, n que seja possivel 50 linhas com "function"
    int quantidade_funcoes = 0; // vai servir de indicie para colocar no inicio_funcoes
    while (fscanf(f, " %[^\n]", linha) != EOF) {
        if (strncmp(linha, "function", 8) == 0) {
            for (int i = 0; i < 4; i++) {          /* o for do prólogo mora AQUI */
                code[idx_codigo] = prologo[i];
                idx_codigo++;
            }
            // TODO: Salvar o endereço (idx_codigo) como o início de uma nova função
            // TODO: Inserir os bytes do prólogo do Assembly
        } 
        else if (strncmp(linha, "end", 3) == 0) {
            // TODO: Inserir os bytes do epílogo do Assembly
        }
        else if (strncmp(linha, "ret", 3) == 0) {
            int valor;
            sscanf(linha, "ret $%d", &valor);       /* tira o número */

            code[idx_codigo] = 0xb8;  idx_codigo++; /* movl $___, %eax */
            code[idx_codigo] = valor & 0xff;          idx_codigo++;
            code[idx_codigo] = (valor >> 8) & 0xff;   idx_codigo++;
            code[idx_codigo] = (valor >> 16) & 0xff;  idx_codigo++;
            code[idx_codigo] = (valor >> 24) & 0xff;  idx_codigo++;

            code[idx_codigo] = epilogo[0];  idx_codigo++;  /* leave */
            code[idx_codigo] = epilogo[1];  idx_codigo++;  /* ret   */
        }
        }
        // TODO: Adicionar if/else para 'zret', 'call' e '='
        
    }
    
    // TODO: Atualizar *entry para apontar para o início da ÚLTIMA função registrada
}