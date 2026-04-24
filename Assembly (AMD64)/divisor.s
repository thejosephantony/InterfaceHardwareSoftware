section .data

section .text

global _start


_start:
    mov rax, 100
    mov rbx, 20
    xor rdx, rdx
    div rbx

    mov rdi, rax
    mov rax, 60
    syscall

; O programa irá terminar com o código de saída 5, que é o resultado da divisão de 100 por 20.