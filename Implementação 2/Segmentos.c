#include <stdint.h>

uint64_t a = 64; // valor inicial de a, que é um número inteiro de 64 bits
uint32_t b; // variável b, que é um número inteiro de 32 bits, onde o resultado do deslocamento será armazenado

int main(){
    // deslocamento a direita em 2 bits, ou seja, divisão por 4
    b = a >> 2;

    // decrementa b em 1 e armazena o resultado em c
    uint16_t c = b - 1;
    
    return c;
}