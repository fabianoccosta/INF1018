.data
S2:
    .byte 65, 108, 111, 32, 123, 103, 97, 108, 101, 114, 97, 125, 33, 0
Sf_char:
    .string "%c"
Sf_newline:
    .string "\n"

.text
.globl main
main:
    pushq %rbp
    movq  %rsp, %rbp

    # Preserva o registrador callee-saved %r12 (nosso ponteiro pc)
    pushq %r12
    leaq  S2(%rip), %r12      # pc = S2

.L_while:
    cmpb  $0, (%r12)          # while (*pc != 0)
    je    .L_fim_while

    # Teste 1: *pc == '{' (123)
    cmpb  $123, (%r12)
    je    .L_proximo          

    # Teste 2: *pc == '}' (125)
    cmpb  $125, (%r12)
    je    .L_proximo          

    # Impressão do caractere válido
    movsbl (%r12), %esi       
    leaq   Sf_char(%rip), %rdi
    movl   $0, %eax
    call   printf@PLT

.L_proximo:
    incq  %r12                # pc++
    jmp   .L_while

.L_fim_while:
    # Imprime quebra de linha
    leaq  Sf_newline(%rip), %rdi
    movl  $0, %eax
    call  printf@PLT

    popq  %r12
    movl  $0, %eax            # return 0
    movq  %rbp, %rsp
    popq  %rbp
    ret