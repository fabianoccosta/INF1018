.data
nums:
    .int 65, -105, 111, 34
Sf_soma:
    .string "soma = %d\n"

.text
.globl main
main:
    pushq %rbp
    movq  %rsp, %rbp

    # Preserva registradores que serão utilizados
    pushq %rbx
    pushq %r12
    pushq %r13

    movq  $0, %rbx             # i = 0 
    movl  $0, %r13d            # s = 0 
    leaq  nums(%rip), %r12     # %r12 = &nums[0]

.L_for_soma:
    cmpl  $4, %ebx             # i < 4 ?
    jge   .L_fim_soma          # Se i >= 4, encerra

    # Acesso indexado: nums[i] -> (%r12, %rbx, 4)
    # Adiciona o valor de nums[i] diretamente no acumulador s (%r13d)
    addl  (%r12, %rbx, 4), %r13d

    incq  %rbx                 # i++
    jmp   .L_for_soma

.L_fim_soma:
    # Imprime "soma = %d\n"
    movl  %r13d, %esi          
    leaq  Sf_soma(%rip), %rdi
    movl  $0, %eax
    call  printf@PLT

    # Restaura os registradores
    popq  %r13
    popq  %r12
    popq  %rbx

    movl  $0, %eax             # return 0
    movq  %rbp, %rsp
    popq  %rbp
    ret