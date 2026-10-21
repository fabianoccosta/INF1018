.data
a: .long 2
b: .long 3
c: .long 4

.text
.globl main
main:
/**********************************************************/
/* mantenha este trecho aqui - prologo !!! */
pushq %rbp                   # Salva o rbp da função anterior na pilha
movq %rsp, %rbp              # Define o topo da pilha atual como o novo rbp
subq $16, %rsp               # Aloca 16 bytes na pilha para os registradores callee-saved
movq %rbx, -8(%rbp)          # Preserva o registrador rbx na pilha (-8)
movq %r12, -16(%rbp)         # Preserva o registrador r12 na pilha (-16)
/**********************************************************/

    movl a(%rip), %eax       # Carrega o valor da variável global 'a' para eax (%eax = 5)
    addl b(%rip), %eax       # Soma o valor da variável global 'b' em eax (%eax = 5 + 10 = 15)
    movl %eax, c(%rip)       # Salva o resultado de eax na variável global 'c' (c = 15)

/**********************************************************/
/* mantenha este trecho aqui - finalizacao !!!! */
movq $0, %rax                # Define o valor de retorno da função como 0 (return 0)
movq -8(%rbp), %rbx          # Restaura o valor original de rbx contido na pilha (-8)
movq -16(%rbp), %r12         # Restaura o valor original de r12 contido na pilha (-16)
leave                        # Desfaz a moldura de pilha (equivale a: movq %rbp, %rsp + popq %rbp)
ret                          # Retorna da função desempilhando o endereço de retorno
/**********************************************************/

