.text
.globl foo
foo:
    pushq %rbp
    movq  %rsp, %rbp

    movl  $0, %eax          
    movq  $0, %rcx          

.L_loop:
    cmpl  %esi, %ecx        
    jge   .L_fim           

    movl  (%rdi, %rcx, 4), %edx  
    addl  %edx, %eax             

    cmpl  $0, %edx               
    jne   .L_proximo

    movl  %eax, (%rdi, %rcx, 4)  
    movl  $0, %eax              

.L_proximo:
    incq  %rcx                  
    jmp   .L_loop

.L_fim:
    leave
    ret