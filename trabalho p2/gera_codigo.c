#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Constante obrigatoria: a constante CEB respeita uma consistencia para a chamada da funcao gera_codigo.c */
const char CEB[] = "CEB"; 

typedef int (*funcp) (int x);

void gera_codigo(FILE *f, unsigned char code[], funcp *entry) {
    char linha[256];
    int idx_codigo = 0; // Índice que controla onde escrever o próximo byte no vetor 'code'
    
    // Você precisará de uma forma de rastrear onde cada função começa
    // para preencher o ponteiro entry no final e para lidar com chamadas (call).
    
    while (fscanf(f, " %[^\n]", linha) != EOF) {
        if (strncmp(linha, "function", 8) == 0) {
            // TODO: Salvar o endereço (idx_codigo) como o início de uma nova função
            // TODO: Inserir os bytes do prólogo do Assembly
        } 
        else if (strncmp(linha, "end", 3) == 0) {
            // TODO: Inserir os bytes do epílogo do Assembly
        }
        else if (strncmp(linha, "ret", 3) == 0) {
            // TODO: Parsear o argumento (var, p0 ou $num) e mover para %eax
        }
        // TODO: Adicionar if/else para 'zret', 'call' e '='
    }
    
    // TODO: Atualizar *entry para apontar para o início da ÚLTIMA função registrada
}