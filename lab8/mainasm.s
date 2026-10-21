.data
nums: .int 3, -5, 7, 8, -2
s1:   .string "%d\n"

.text
.globl main
main:
     pushq %rbp
     movq  %rsp, %rbp
     subq  $16, %rsp
     movq  %rbx, -8(%rbp)
     movq  %r12, -16(%rbp)

     leaq  nums(%rip), %rbx
     movl  $0, %r12d

.L_loop:
     cmpl  $5, %r12d
     jge   .L_fim

     movl  (%rbx), %edi
     movl  $1, %esi
     call  filtro@PLT

     movl  %eax, %esi
     leaq  s1(%rip), %rdi
     movl  $0, %eax
     call  printf@PLT

     addq  $4, %rbx
     incl  %r12d
     jmp   .L_loop

.L_fim:
     movl  $0, %eax

     movq  -8(%rbp), %rbx
     movq  -16(%rbp), %r12
     leave
     ret