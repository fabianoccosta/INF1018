.data
s1: .string "ls -ls"

.text
.globl main
main:
    pushq %rbp
    movq  %rsp, %rbp

    leaq  s1(%rip), %rdi
    movl  $0, %eax
    call  system@PLT

    movl  $0, %eax

    leave
    ret