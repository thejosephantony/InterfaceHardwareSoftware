.intel_syntax noprefix
.extern fopen, fclose, fscanf, fprintf, printf, malloc, free, calloc

.section .data
modo_r:     .asciz "r"
modo_w:     .asciz "w"
fmt_hex:    .asciz "%x"
fmt_dec:    .asciz "%d"
fmt_tripla: .asciz "%x %x %x"
fmt_char:   .asciz "%c"
fmt_taxa:   .asciz "[%d.%02d%%]\n"
err_msg:    .asciz "Error\n"

.section .bss
.align 16
paleta:     .space 16
v_tmp:      .long 0
v_nimg:     .long 0
v_ntup:     .long 0
v_larg:     .long 0
v_alt:      .long 0
f_in:       .quad 0
f_out:      .quad 0
matriz_ptr: .quad 0
tuplas_ptr: .quad 0
maxrow_ptr: .quad 0

.section .text
.globl main

main:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8

    mov r12, rsi
    cmp rdi, 3
    jl erro_geral

    mov rdi, [r12+8]
    lea rsi, [rip+modo_r]
    call fopen
    mov [rip+f_in], rax
    test rax, rax
    jz erro_geral

    mov rdi, [r12+16]
    lea rsi, [rip+modo_w]
    call fopen
    mov [rip+f_out], rax
    test rax, rax
    jz erro_fecha_in

    xor r14d, r14d

loop_paleta:
    cmp r14d, 16
    jge le_nimg

    mov rdi, [rip+f_in]
    lea rsi, [rip+fmt_hex]
    lea rdx, [rip+v_tmp]
    xor eax, eax
    call fscanf

    cmp eax, 1
    jne erro_fecha_ambos

    mov eax, [rip+v_tmp]
    lea rcx, [rip+paleta]
    mov [rcx+r14], al

    inc r14d
    jmp loop_paleta

le_nimg:
    mov rdi, [rip+f_in]
    lea rsi, [rip+fmt_dec]
    lea rdx, [rip+v_nimg]
    xor eax, eax
    call fscanf

    cmp eax, 1
    jne erro_fecha_ambos

    mov r12d, [rip+v_nimg]

loop_imagens:
    test r12d, r12d
    jz sucesso_fim

    mov qword ptr [rip+matriz_ptr], 0
    mov qword ptr [rip+tuplas_ptr], 0
    mov qword ptr [rip+maxrow_ptr], 0

    mov rdi, [rip+f_in]
    lea rsi, [rip+fmt_dec]
    lea rdx, [rip+v_ntup]
    xor eax, eax
    call fscanf

    cmp eax, 1
    jne sucesso_fim

    mov dword ptr [rip+v_larg], 0
    mov dword ptr [rip+v_alt], 0

    mov eax, [rip+v_ntup]
    test eax, eax
    jz next_img

    movsxd rdi, dword ptr [rip+v_ntup]
    imul rdi, 12
    call malloc

    test rax, rax
    jz erro_fecha_ambos

    mov [rip+tuplas_ptr], rax
    xor r13d, r13d

le_tuplas:
    mov eax, [rip+v_ntup]
    cmp r13d, eax
    jge fim_leitura

    mov rbx, [rip+tuplas_ptr]
    mov eax, r13d
    imul eax, 12
    add rbx, rax

    mov rdi, [rip+f_in]
    lea rsi, [rip+fmt_tripla]
    lea rdx, [rbx]
    lea rcx, [rbx+4]
    lea r8,  [rbx+8]
    xor eax, eax
    call fscanf

    cmp eax, 3
    jne fim_leitura_parcial

    mov edx, dword ptr [rbx+4]
    inc edx
    cmp edx, [rip+v_alt]
    jle prox_le
    mov [rip+v_alt], edx

prox_le:
    inc r13d
    jmp le_tuplas

fim_leitura_parcial:
    mov [rip+v_ntup], r13d

fim_leitura:
    mov eax, [rip+v_alt]
    test eax, eax
    jz prox_img

    movsxd rdi, dword ptr [rip+v_alt]
    mov rsi, 4
    call calloc

    test rax, rax
    jz prox_img

    mov [rip+maxrow_ptr], rax

    xor r13d, r13d

calc_maxrow:
    mov eax, [rip+v_ntup]
    cmp r13d, eax
    jge fim_maxrow

    mov rbx, [rip+tuplas_ptr]
    mov eax, r13d
    imul eax, 12
    add rbx, rax

    mov eax, dword ptr [rbx+8]
    shr eax, 4
    add eax, dword ptr [rbx]

    mov edx, dword ptr [rbx+4]
    mov rcx, [rip+maxrow_ptr]

    cmp eax, [rcx+rdx*4]
    jle prox_maxrow
    mov [rcx+rdx*4], eax

prox_maxrow:
    inc r13d
    jmp calc_maxrow

fim_maxrow:
    mov eax, 0x7fffffff
    xor r13d, r13d
    mov rcx, [rip+maxrow_ptr]

calc_larg:
    mov edx, [rip+v_alt]
    cmp r13d, edx
    jge fim_larg

    mov edx, [rcx+r13*4]
    test edx, edx
    jz prox_larg

    cmp edx, eax
    jge prox_larg
    mov eax, edx

prox_larg:
    inc r13d
    jmp calc_larg

fim_larg:
    cmp eax, 0x7fffffff
    jne salva_larg
    mov eax, 1

salva_larg:
    mov [rip+v_larg], eax

    mov rdi, [rip+maxrow_ptr]
    test rdi, rdi
    jz aloca_matriz
    call free
    mov qword ptr [rip+maxrow_ptr], 0

aloca_matriz:
    movsxd rax, dword ptr [rip+v_larg]
    movsxd rcx, dword ptr [rip+v_alt]
    imul rax, rcx

    test rax, rax
    jz prox_img

    mov rdi, rax
    call malloc

    test rax, rax
    jz prox_img

    mov [rip+matriz_ptr], rax

    mov rdi, rax
    mov al, byte ptr [rip+paleta]

    movsxd rcx, dword ptr [rip+v_larg]
    movsxd rdx, dword ptr [rip+v_alt]
    imul rcx, rdx

    rep stosb

    xor r13d, r13d

pinta_loop:
    mov eax, [rip+v_ntup]
    cmp r13d, eax
    jge renderiza

    mov rbx, [rip+tuplas_ptr]
    mov eax, r13d
    imul eax, 12
    add rbx, rax

    mov r8d,  [rbx]
    mov r9d,  [rbx+4]
    mov r10d, [rbx+8]

    mov eax, r9d
    imul eax, [rip+v_larg]
    add eax, r8d

    movsxd rcx, dword ptr [rip+v_larg]
    movsxd rdx, dword ptr [rip+v_alt]
    imul rcx, rdx

    cmp rax, rcx
    jae prox_pinta

    mov edx, r10d
    shr edx, 4
    test edx, edx
    jz prox_pinta

    and r10d, 0x0f
    lea rcx, [rip+paleta]
    movzx r11d, byte ptr [rcx+r10]

    mov rdi, [rip+matriz_ptr]
    add rdi, rax

    movsxd rax, dword ptr [rip+v_larg]
    movsxd rcx, dword ptr [rip+v_alt]
    imul rax, rcx

    mov r15, [rip+matriz_ptr]
    add r15, rax

pinta_cel:
    test edx, edx
    jz prox_pinta

    cmp rdi, r15
    jae prox_pinta

    mov [rdi], r11b

    inc rdi
    dec edx
    jmp pinta_cel

prox_pinta:
    inc r13d
    jmp pinta_loop

renderiza:
    movsxd rax, dword ptr [rip+v_ntup]
    imul rax, 30000

    movsxd rcx, dword ptr [rip+v_larg]
    movsxd rdx, dword ptr [rip+v_alt]
    imul rcx, rdx

    test rcx, rcx
    jz prox_img

    mov rbx, rcx
    mov rdx, rbx
    shr rdx, 1
    add rax, rdx

    cqo
    idiv rbx

    mov rbx, 100
    cqo
    idiv rbx

    mov r8d, edx

    mov rdi, [rip+f_out]
    lea rsi, [rip+fmt_taxa]
    mov edx, eax
    mov ecx, r8d
    xor eax, eax
    call fprintf

    xor r14d, r14d

loop_y:
    mov eax, [rip+v_alt]
    cmp r14d, eax
    jge prox_img

    xor r15d, r15d

loop_x:
    mov eax, [rip+v_larg]
    cmp r15d, eax
    jge nova_linha

    mov eax, r14d
    imul eax, [rip+v_larg]
    add eax, r15d

    mov rbx, [rip+matriz_ptr]
    movzx edx, byte ptr [rbx+rax]

    mov rdi, [rip+f_out]
    lea rsi, [rip+fmt_char]
    xor eax, eax
    call fprintf

    inc r15d
    jmp loop_x

nova_linha:
    mov rdi, [rip+f_out]
    lea rsi, [rip+fmt_char]
    mov edx, 10
    xor eax, eax
    call fprintf

    inc r14d
    jmp loop_y

prox_img:
    mov rdi, [rip+maxrow_ptr]
    test rdi, rdi
    jz free_matriz
    call free
    mov qword ptr [rip+maxrow_ptr], 0

free_matriz:
    mov rdi, [rip+matriz_ptr]
    test rdi, rdi
    jz free_tuplas
    call free
    mov qword ptr [rip+matriz_ptr], 0

free_tuplas:
    mov rdi, [rip+tuplas_ptr]
    test rdi, rdi
    jz next_img
    call free
    mov qword ptr [rip+tuplas_ptr], 0

next_img:
    dec r12d
    jmp loop_imagens

sucesso_fim:
    mov rdi, [rip+f_in]
    test rdi, rdi
    jz fecha_out
    call fclose

fecha_out:
    mov rdi, [rip+f_out]
    test rdi, rdi
    jz saida_ok
    call fclose
    jmp saida_ok

erro_fecha_ambos:
    mov rdi, [rip+f_out]
    test rdi, rdi
    jz erro_fecha_in
    call fclose

erro_fecha_in:
    mov rdi, [rip+f_in]
    test rdi, rdi
    jz erro_geral
    call fclose

erro_geral:
    lea rdi, [rip+err_msg]
    xor eax, eax
    call printf

    mov eax, 1
    jmp saida_final

saida_ok:
    xor eax, eax

saida_final:
    add rsp, 8
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret

.section .note.GNU-stack,"",@progbits