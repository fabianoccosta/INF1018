.text
.globl add
add:
    pushq %rbp
    movq  %rsp, %rbp

    addl  %esi, %edi
    addl  %edx, %edi
    movl  %edi, %eax

    leave
    ret