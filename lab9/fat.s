.text
.globl fat
fat:
    pushq %rbp
    movq  %rsp, %rbp
    pushq %rbx              

    movl  %edi, %ebx        

    cmpl  $0, %ebx          
    jne   .L_rec

    movl  $1, %eax        
    jmp   .L_fim

.L_rec:
    decl  %edi             
    call  fat@PLT           

    imull %ebx, %eax       

.L_fim:
    popq  %rbx              
    leave
    ret