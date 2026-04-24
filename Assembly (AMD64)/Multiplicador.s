section .data

section .text

global _start

_start:
    mov rax, 5
    mov rbx, 10
    imul rax, rbx      ; rax = 50

    mov rdi, rax
    mov rax, 60
    syscall

; O programa irá terminar com o código de saída 50, que é o resultado da multiplicação de 5 e 10.