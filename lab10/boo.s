# void boo (struct X *px, int n, int val) {
#   while (n--) {
#     px->val2 = f(px->val1, val);
#     px++;
#   }
# }
# struct X { int val1; /* 0 */ int val2; /* 4 */ };   sizeof = 8
#
# px  -> %rdi  (guardado em %r12)
# n   -> %esi  (guardado em %r13d)
# val -> %edx  (guardado em %r14d)

.text
.globl boo
boo:
    pushq   %r12
    pushq   %r13
    pushq   %r14                # 3 pushes: pilha alinhada em 16 para os calls

    movq    %rdi, %r12          # r12 = px
    movl    %esi, %r13d         # r13d = n
    movl    %edx, %r14d         # r14d = val
L1:
    movl    %r13d, %eax         # eax = n (valor antigo)
    subl    $1, %r13d           # n--
    testl   %eax, %eax          # while (n antigo != 0)
    je      L2
    movl    (%r12), %edi        # 1o arg: px->val1
    movl    %r14d, %esi         # 2o arg: val
    call    f
    movl    %eax, 4(%r12)       # px->val2 = f(...)
    addq    $8, %r12            # px++
    jmp     L1
L2:
    popq    %r14
    popq    %r13
    popq    %r12
    ret