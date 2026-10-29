# void map2 (int* um, int * outro, int n) {
#   int i;
#   for (i=0; i<n; i++)
#     *(outro+i) = f(*(um+i));
# }
#
# um    -> %rdi (guardado em %r12)
# outro -> %rsi (guardado em %r13)
# n     -> %edx (guardado em %r14d)
# i     -> %ebx

.text
.globl map2
map2:
    pushq   %rbx
    pushq   %r12
    pushq   %r13
    pushq   %r14
    subq    $8, %rsp            # 4 pushes + 8 bytes: pilha alinhada em 16

    movq    %rdi, %r12          # r12 = um
    movq    %rsi, %r13          # r13 = outro
    movl    %edx, %r14d         # r14d = n
    movl    $0, %ebx            # i = 0
L1:
    cmpl    %r14d, %ebx         # i < n ?
    jge     L2
    movslq  %ebx, %rax          # rax = (long) i
    movl    (%r12,%rax,4), %edi # edi = um[i]
    call    f                   # eax = f(um[i])
    movslq  %ebx, %rcx          # rcx = (long) i (rcx nao sobrevive ao call)
    movl    %eax, (%r13,%rcx,4) # outro[i] = eax
    addl    $1, %ebx            # i++
    jmp     L1
L2:
    addq    $8, %rsp
    popq    %r14
    popq    %r13
    popq    %r12
    popq    %rbx
    ret