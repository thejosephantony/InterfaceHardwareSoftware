section .data

section .text
global _start

_start:
    mov rax, 20
    mov rbx, 10
    sub rax, rbx     ; rax = 10

    mov rdi, rax     ; move o resultado para rdi (argumento para exit)
    mov rax, 60
    syscall           ; chamada do kernel para sair do programa
; O programa irá terminar com o código de saída 10, que é a diferença entre 20 e 10.