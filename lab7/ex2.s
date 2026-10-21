.data
Sf:
    .string "%d\n"

.text
.globl main
main:
    pushq %rbp
    movq  %rsp, %rbp

    pushq %rbx                 # i
    movl  $1, %ebx             # i = 1

.L_loop_quad:
    cmpl  $10, %ebx            # i <= 10 
    jg    .L_fim_quad          # Se i > 10, encerra

    # Calcula i * i
    movl  %ebx, %eax           # %eax = i
    imull %eax, %eax           # %eax = i * i

    # Imprime o resultado
    movl  %eax, %esi           # 1 argumento do printf
    leaq  Sf(%rip), %rdi       # 2 argumento do printf
    movl  $0, %eax
    call  printf@PLT

    incl  %ebx                 # i++
    jmp   .L_loop_quad

.L_fim_quad:
    popq  %rbx
    movl  $0, %eax             # return 0
    movq  %rbp, %rsp
    popq  %rbp
    ret