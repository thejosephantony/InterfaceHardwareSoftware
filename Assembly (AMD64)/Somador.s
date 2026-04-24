section .data

section .text
global _start

_start:
    mov rax, 10
    mov rbx, 20
    add rax, rbx      ; rax = 30

    mov rdi, rax      ; move o resultado para rdi (argumento para exit)
    mov rax, 60       ; syscall exit
    syscall
; O programa irá terminar com o código de saída 30, que é a soma de 10 e 20.