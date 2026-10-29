# int add (struct X *x)
# struct X { int val; /* 0 */ struct X *next; /* 8 */ };
#
# x -> %rdi
# a -> %eax (valor de retorno)

.text
.globl add
add:
    movl    $0, %eax            # a = 0
L1:
    cmpq    $0, %rdi            # x != NULL ?
    je      L2
    addl    (%rdi), %eax        # a += x->val
    movq    8(%rdi), %rdi       # x = x->next
    jmp     L1
L2:
    ret