# int add2 (struct X *x) {
#   if (x == NULL) return 0;
#   else return x->val + add2(x->next);
# }
#
# x -> %rdi
# %rbx (callee-saved) guarda x->val durante a chamada recursiva

.text
.globl add2
add2:
    cmpq    $0, %rdi            # x == NULL ?
    jne     L1
    movl    $0, %eax            # return 0
    ret
L1:
    pushq   %rbx                # salva %rbx (tambem alinha a pilha em 16)
    movl    (%rdi), %ebx        # ebx = x->val
    movq    8(%rdi), %rdi       # rdi = x->next
    call    add2                # eax = add2(x->next)
    addl    %ebx, %eax          # eax = x->val + add2(x->next)
    popq    %rbx                # restaura %rbx
    ret