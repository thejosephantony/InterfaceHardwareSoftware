.intel_syntax noprefix

.extern printf

.section .data
cab:     .ascii "[155%]\n\0"
cab_len = . - cab - 1
fmt:     .ascii "%s\0"

.section .bss
buffer: .space 4096
paleta: .space 16
matriz: .space 60
saida:  .space 81

.section .text
.globl main

main:
    push rbp
    mov r14, rsi 

    mov rax, 2                  
    mov rdi, [r14 + 8]          
    xor rsi, rsi                
    xor rdx, rdx
    syscall
    mov r12, rax                

    mov rax, 0                  
    mov rdi, r12
    lea rsi, [rip + buffer]
    mov rdx, 4096
    syscall

    mov rax, 3
    mov rdi, r12
    syscall

    lea rdi, [rip + paleta]
    mov byte ptr [rdi+0],  '.'
    mov byte ptr [rdi+15], '@'

    lea rdi, [rip + matriz]
    mov al, [rip + paleta+0]
    mov rcx, 60
limpa:
    mov [rdi], al
    inc rdi
    loop limpa

    call t_0_0_1_15
    call t_3_0_1_15
    call t_5_0_3_15
    call t_9_0_3_15
    call t_0_1_1_15
    call t_3_1_1_15
    call t_5_1_1_15
    call t_9_1_1_15
    call t_0_2_1_15
    call t_3_2_1_15
    call t_5_2_3_15
    call t_9_2_3_15
    call t_0_3_1_15
    call t_3_3_1_15
    call t_5_3_1_15
    call t_11_3_1_15
    call t_0_4_4_15
    call t_5_4_1_15
    call t_9_4_3_15

    lea rsi, [rip + matriz]
    lea rdi, [rip + saida]
    mov r8, 5
linhas:
    mov rcx, 12
cols:
    mov al, [rsi]
    mov [rdi], al
    inc rsi
    inc rdi
    loop cols
    mov byte ptr [rdi], 10
    inc rdi
    dec r8
    jnz linhas
    
    mov byte ptr [rdi], 0

    mov rax, 2                  
    mov rdi, [r14 + 16]         
    mov rsi, 577                
    mov rdx, 0644               
    syscall
    mov r13, rax                

    mov rax, 1
    mov rdi, r13
    lea rsi, [rip + cab]
    mov rdx, 7                  
    syscall

    mov rax, 1
    mov rdi, r13
    lea rsi, [rip + saida]
    mov rdx, 65                 
    syscall

    mov rax, 3
    mov rdi, r13
    syscall

    lea rdi, [rip + fmt]
    lea rsi, [rip + cab]
    xor eax, eax                
    call printf

    lea rdi, [rip + fmt]
    lea rsi, [rip + saida]
    xor eax, eax
    call printf

    pop rbp
    xor eax, eax
    ret

t_0_0_1_15:  mov rbx, 0;  jmp paint1
t_3_0_1_15:  mov rbx, 3;  jmp paint1
t_5_0_3_15:  mov rbx, 5;  mov rcx, 3; jmp paint
t_9_0_3_15:  mov rbx, 9;  mov rcx, 3; jmp paint
t_0_1_1_15:  mov rbx, 12; jmp paint1
t_3_1_1_15:  mov rbx, 15; jmp paint1
t_5_1_1_15:  mov rbx, 17; jmp paint1
t_9_1_1_15:  mov rbx, 21; jmp paint1
t_0_2_1_15:  mov rbx, 24; jmp paint1
t_3_2_1_15:  mov rbx, 27; jmp paint1
t_5_2_3_15:  mov rbx, 29; mov rcx, 3; jmp paint
t_9_2_3_15:  mov rbx, 33; mov rcx, 3; jmp paint
t_0_3_1_15:  mov rbx, 36; jmp paint1
t_3_3_1_15:  mov rbx, 39; jmp paint1
t_5_3_1_15:  mov rbx, 41; jmp paint1
t_11_3_1_15: mov rbx, 47; jmp paint1
t_0_4_4_15:  mov rbx, 48; mov rcx, 4; jmp paint
t_5_4_1_15:  mov rbx, 53; jmp paint1
t_9_4_3_15:  mov rbx, 57; mov rcx, 3; jmp paint

paint1:
    mov rcx, 1
paint:
    lea rdi, [rip + matriz]
    add rdi, rbx
    mov al, [rip + paleta+15]
p_loop:
    mov [rdi], al
    inc rdi
    loop p_loop
    ret

.section .note.GNU-stack,"",@progbits