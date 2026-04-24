section .data

section .text

global _start


_start:
    mov rax, 60        ; syscall exit
    mov rdi, 0         ; codigo de saida
    syscall            ; chamada do kernel


; primeiras instruções do programa
; movimentação de dados, operações aritméticas, etc.
