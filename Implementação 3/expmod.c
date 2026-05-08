#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

uint64_t expmod(const uint64_t b, const uint64_t n, const uint64_t p) {
    __uint128_t a = 1;

    for (uint64_t i = 0; i < n; i++) {
        a = (a * b) % p;
    }

    return (uint64_t)a;
}

int main() {
    uint64_t resultado = expmod(
        2,
        123456789,
        18446744073709551533ULL
    );

    printf("2^123456789 mod 18446744073709551533 = %" PRIu64 "\n", resultado);

    return 0;
}

/*joseph@JosephPC:/mnt/c/Users/Joseph$ gcc -Wall -O3 expmod.c -o expmod.elf
joseph@JosephPC:/mnt/c/Users/Joseph$ ./expmod.elf
2^123456789 mod 18446744073709551533 = 7398179800986853190
joseph@JosephPC:/mnt/c/Users/Joseph$ time ./expmod.elf
2^123456789 mod 18446744073709551533 = 7398179800986853190

real    0m1.068s
user    0m1.053s
sys     0m0.003s 


joseph@JosephPC:/mnt/c/Users/Joseph$ for i in {1..5}; do time ./expmod.elf; done
2^123456789 mod 18446744073709551533 = 7398179800986853190

real    0m1.004s
user    0m0.985s
sys     0m0.008s
2^123456789 mod 18446744073709551533 = 7398179800986853190

real    0m1.030s
user    0m1.023s
sys     0m0.004s
2^123456789 mod 18446744073709551533 = 7398179800986853190

real    0m1.149s
user    0m1.115s
sys     0m0.029s
2^123456789 mod 18446744073709551533 = 7398179800986853190

real    0m0.970s
user    0m0.962s
sys     0m0.004s
2^123456789 mod 18446744073709551533 = 7398179800986853190

real    0m0.986s
user    0m0.954s
sys     0m0.028s
joseph@JosephPC:/mnt/c/Users/Joseph$

joseph@JosephPC:/mnt/c/Users/Joseph$ hyperfine --warmup 3 './expmod.elf'
Benchmark 1: ./expmod.elf
  Time (mean ± σ):      1.100 s ±  0.127 s    [User: 1.081 s, System: 0.003 s]
  Range (min … max):    0.980 s …  1.351 s    10 runs

A implementação AOT da exponenciação modular foi compilada com gcc utilizando a flag -O3. A execução produziu corretamente o resultado 7398179800986853190 para a operação 2^123456789 mod 18446744073709551533. A medição com hyperfine indicou tempo médio de 1.100 s ± 0.127 s em 10 execuções, enquanto a medição simples com time apresentou tempo real de 1.068 s.
*/

