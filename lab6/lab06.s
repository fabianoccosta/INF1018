.data
# Questão 3: Declaração do array de bytes (.byte)
nums:
    .byte 10, -21, -30, 45

.text
.globl main
main:
    pushq %rbp
    movq %rsp, %rbp

    # Preserva o registrador callee-saved %rbx / %r12
    pushq %rbx
    pushq %r12

    # Questão 1: Inicialização do contador (i em %rbx) e endereço base de nums em %r12
    movq $0, %rbx          
    leaq nums(%rip), %r12 

.L_loop:
    # Questão 1: Condição de parada do loop (i != 4)
    cmpl $4, %ebx         
    je .L_fim             

    # Questão 3: Acesso ao array de bytes com extensão de sinal usando %rbx (64 bits)
    movsbl (%r12, %rbx), %edx  

    # Questão 2: Teste lógico bit a bit para verificar se é par
    testl $1, %edx
    jnz .L_continua       

    # Questão 2: Move o número par para %eax para impressão
    movl %edx, %eax

    # Trecho de Impressão
    pushq %rdi
    pushq %rsi
    movl  %eax, %esi
    leaq  .LC0(%rip), %rdi
    movl  $0, %eax
    call  printf@PLT
    popq  %rsi
    popq  %rdi

.L_continua:
    # Questão 1 & 2: Incremento do contador (i++)
    incl %ebx             
    jmp .L_loop           

.L_fim:
    # Restaura os registradores preservados
    popq %r12
    popq %rbx

    movl $0, %eax
    movq %rbp, %rsp
    popq %rbp
    ret

.section .rodata
.LC0:
    .string "%d\n"