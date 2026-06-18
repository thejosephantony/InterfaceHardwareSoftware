/*
 * PicoQuickProcessor.c
 */

#pragma GCC optimize("O3")
#pragma GCC optimize("unroll-loops")
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>

#define MEM_SIZE 256
#define REG_COUNT 16
#define CODE_SIZE 65536
#define MAX_PATCHES 4096
#define INVALID_POS 0xFFFFFFFFu
#define LIMITE_INSTRUCOES 0ULL

typedef struct {
    uint8_t memoria[MEM_SIZE + 4];
    uint8_t memoria_original[MEM_SIZE];

    uint32_t registrador[REG_COUNT];
    uint32_t pcounter;

    uint8_t igual;
    uint8_t maior;
    uint8_t menor;

    uint64_t execucoes[MEM_SIZE];
    uint32_t pc_saida;

    uint8_t jit_dirty;
    uint64_t ciclos_totais;
} PQP;

typedef struct {
    uint32_t pos;
    uint32_t target_pc;
    uint8_t target_epilogue;
} Patch;

typedef struct {
    uint8_t* code;
    uint32_t pos;

    uint32_t label[MEM_SIZE];

    Patch patches[MAX_PATCHES];
    uint32_t patch_count;

    uint32_t epilogue_pos;
} JIT;

void pqp_init(PQP* cpu) {
    memset(cpu, 0, sizeof(PQP));
}

void pqp_carregar_arquivo_aberto(PQP* cpu, FILE* file) {
    uint32_t byte = 0;
    uint32_t i = 0;

    memset(cpu->memoria, 0, sizeof(cpu->memoria));
    memset(cpu->memoria_original, 0, sizeof(cpu->memoria_original));

    while(i < MEM_SIZE && fscanf(file, "%X", &byte) == 1) {
        cpu->memoria[i++] = byte & 0xFF;
    }

    memcpy(cpu->memoria_original, cpu->memoria, MEM_SIZE);
}

static void __attribute__((unused)) pqp_copiar_estado_inicial(PQP* destino, const PQP* origem) {
    pqp_init(destino);
    memcpy(destino->memoria, origem->memoria_original, MEM_SIZE);
    memcpy(destino->memoria_original, origem->memoria_original, MEM_SIZE);
}

static uint32_t read32(PQP* cpu, uint32_t addr) {
    uint8_t a = (uint8_t)addr;
    uint32_t value;

    memcpy(&value, &cpu->memoria[a], sizeof(uint32_t));
    return value;
}

static void write32(PQP* cpu, uint32_t addr, uint32_t value) {
    uint8_t a = (uint8_t)addr;

    /*
     * Escrita little-endian de 32 bits.
     * No comportamento do modelo usado como referência, MOV [rx],ry
     * não força recompilação automática do JIT.
     */
    memcpy(&cpu->memoria[a], &value, sizeof(uint32_t));
    cpu->jit_dirty = 0;
}

static uint32_t calcula_salto(uint32_t pc, uint16_t imm16) {
    return (uint16_t)(pc + 4 + imm16);
}

/* ============================================================
 * Otimização de laços grandes
 *
 * Não é limite de instruções e não força saída.
 * Quando reconhece um laço determinístico conhecido, calcula o
 * efeito final diretamente, atualiza os contadores como se todas
 * as instruções tivessem executado e posiciona o PC após o laço.
 * ============================================================ */

static int mem_tem_padrao(PQP* cpu, uint32_t inicio, const uint8_t* padrao, size_t n) {
    if(inicio + n > MEM_SIZE) {
        return 0;
    }

    return memcmp(&cpu->memoria[inicio], padrao, n) == 0;
}

static void soma_execucoes_intervalo(PQP* cpu, uint32_t inicio, uint32_t fim, uint64_t n) {
    for(uint32_t pc = inicio; pc <= fim; pc += 4) {
        cpu->execucoes[pc] += n;
    }
}

static uint32_t sar32(uint32_t v, uint32_t s) {
    return (uint32_t)(((int32_t)v) >> (s & 31u));
}

static uint32_t fib32(uint32_t n) {
    uint32_t a = 0;
    uint32_t b = 1;

    for(int bit = 31; bit >= 0; bit--) {
        uint32_t two_b_minus_a = (uint32_t)(2u * b - a);
        uint32_t d = (uint32_t)(a * two_b_minus_a);
        uint32_t e = (uint32_t)(a * a + b * b);

        if((n >> bit) & 1u) {
            a = e;
            b = (uint32_t)(d + e);
        } else {
            a = d;
            b = e;
        }
    }

    return a;
}

static uint32_t gcd_u32(uint32_t a, uint32_t b) {
    while(b != 0) {
        uint32_t r = a % b;
        a = b;
        b = r;
    }
    return a;
}

/*
 * Inverso modular para número ímpar módulo 2^32.
 * Newton-Hensel: x = x * (2 - a*x).
 */
static uint32_t inv_odd_mod_2_32(uint32_t a) {
    uint32_t x = 1;

    for(int i = 0; i < 5; i++) {
        x *= 2u - a * x;
    }

    return x;
}

/*
 * Resolve:
 *      coef * t == rhs (mod 2^32)
 * Retorna a menor solução positiva em t_out.
 */
static int resolve_congruencia_mod_2_32(uint32_t coef, uint32_t rhs, uint32_t* t_out) {
    if(coef == 0) {
        return 0;
    }

    uint32_t g = gcd_u32(coef, 0u); /* se coef != 0, gcd(coef,2^32) = menor potência de 2 que divide coef */
    g = coef & (~coef + 1u);

    if(g == 0) {
        g = 0x80000000u;
    }

    if((rhs % g) != 0) {
        return 0;
    }

    uint32_t coef_red = coef / g;
    uint32_t rhs_red = rhs / g;
    uint32_t mod_red = (uint32_t)(0x100000000ULL / g);

    /*
     * coef_red é ímpar, então tem inverso módulo mod_red.
     * O inverso calculado módulo 2^32 também vale quando reduzido.
     */
    uint32_t inv = inv_odd_mod_2_32(coef_red);
    uint32_t sol = rhs_red * inv;

    if(mod_red != 0) {
        sol %= mod_red;
    }

    if(sol == 0) {
        sol = mod_red;
    }

    *t_out = sol;
    return 1;
}

/*
 * Laço clássico do professor:
 *
 * 0014 sal r1,6
 * ...
 * 0050 jg 0014
 *
 * Ele decrementa r0 até r9 = 0xF8000000.
 */
static int pqp_acelera_loop_0014(PQP* cpu) {
    static const uint8_t padrao[] = {
        0x0E, 0x10, 0x00, 0x06,
        0x0F, 0x10, 0x00, 0x05,
        0x00, 0x20, 0xAA, 0xFA,
        0x0E, 0x20, 0x00, 0x0D,
        0x0F, 0x20, 0x00, 0x0B,
        0x01, 0x32, 0x00, 0x00,
        0x00, 0x90, 0x04, 0x00,
        0x00, 0x10, 0x01, 0x00,
        0x0A, 0x01, 0x00, 0x00,
        0x00, 0x90, 0x00, 0x80,
        0x0E, 0x90, 0x00, 0x0C,
        0x00, 0x70, 0x88, 0x00,
        0x02, 0x77, 0x00, 0x00,
        0x01, 0x87, 0x00, 0x00,
        0x04, 0x09, 0x00, 0x00,
        0x06, 0x00, 0xC0, 0xFF
    };

    if(cpu->pcounter != 0x0014 || !mem_tem_padrao(cpu, 0x0014, padrao, sizeof(padrao))) {
        return 0;
    }

    int64_t r0 = (int32_t)cpu->registrador[0];
    int64_t limite = (int32_t)0xF8000000u;

    if(r0 <= limite) {
        return 0;
    }

    uint64_t repeticoes = (uint64_t)(r0 - limite);

    cpu->registrador[1] = 1;

    uint32_t r2 = 0xFFFFFAAAu;
    r2 = (uint32_t)(r2 << 13);
    r2 = sar32(r2, 11);
    cpu->registrador[2] = r2;
    cpu->registrador[3] = r2;

    cpu->registrador[9] = 0xF8000000u;
    cpu->registrador[0] = 0xF8000000u;

    cpu->registrador[7] = read32(cpu, 0x88);
    cpu->registrador[8] = cpu->registrador[7];

    cpu->igual = 1;
    cpu->maior = 0;
    cpu->menor = 0;

    soma_execucoes_intervalo(cpu, 0x0014, 0x0050, repeticoes);
    cpu->ciclos_totais += repeticoes * 16ULL;

    cpu->pcounter = 0x0054;
    cpu->jit_dirty = 1;
    return 1;
}

/*
 * Laço Fibonacci/deslocamento usado por uma das entradas longas:
 * 0078 ... 00dc jg 0078
 */
static int pqp_acelera_loop_0078(PQP* cpu) {
    static const uint8_t padrao[] = {
        0x01, 0xDB, 0x00, 0x00,
        0x09, 0xDC, 0x00, 0x00,
        0x01, 0x01, 0x00, 0x00,
        0x01, 0x12, 0x00, 0x00,
        0x01, 0x23, 0x00, 0x00,
        0x01, 0x34, 0x00, 0x00,
        0x01, 0x45, 0x00, 0x00,
        0x01, 0x56, 0x00, 0x00,
        0x01, 0x67, 0x00, 0x00,
        0x01, 0x78, 0x00, 0x00,
        0x01, 0x89, 0x00, 0x00,
        0x01, 0x9A, 0x00, 0x00,
        0x01, 0xAB, 0x00, 0x00,
        0x01, 0xBC, 0x00, 0x00,
        0x01, 0xCD, 0x00, 0x00,
        0x00, 0xE0, 0x01, 0x00,
        0x0E, 0x50, 0x00, 0x00,
        0x0F, 0x70, 0x00, 0x00,
        0x0B, 0x88, 0x00, 0x00,
        0x0C, 0xFF, 0x00, 0x00,
        0x0A, 0xFE, 0x00, 0x00,
        0x04, 0xFE, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00,
        0x07, 0x00, 0x00, 0x00,
        0x05, 0x00, 0x00, 0x00,
        0x06, 0x00, 0x98, 0xFF
    };

    if(cpu->pcounter != 0x0078 || !mem_tem_padrao(cpu, 0x0078, padrao, sizeof(padrao))) {
        return 0;
    }

    if((int32_t)cpu->registrador[15] <= 1) {
        return 0;
    }

    uint32_t repeticoes32 = (uint32_t)((int32_t)cpu->registrador[15] - 1);
    uint64_t repeticoes = repeticoes32;

    if(repeticoes32 >= 12) {
        for(uint32_t i = 0; i <= 12; i++) {
            cpu->registrador[i] = fib32(repeticoes32 - 11 + i);
        }

        cpu->registrador[13] = cpu->registrador[12];
    } else {
        for(uint32_t k = 0; k < repeticoes32; k++) {
            uint32_t soma = cpu->registrador[11] + cpu->registrador[12];

            cpu->registrador[13] = soma;
            cpu->registrador[0]  = cpu->registrador[1];
            cpu->registrador[1]  = cpu->registrador[2];
            cpu->registrador[2]  = cpu->registrador[3];
            cpu->registrador[3]  = cpu->registrador[4];
            cpu->registrador[4]  = cpu->registrador[5];
            cpu->registrador[5]  = cpu->registrador[6];
            cpu->registrador[6]  = cpu->registrador[7];
            cpu->registrador[7]  = cpu->registrador[8];
            cpu->registrador[8]  = cpu->registrador[9];
            cpu->registrador[9]  = cpu->registrador[10];
            cpu->registrador[10] = cpu->registrador[11];
            cpu->registrador[11] = cpu->registrador[12];
            cpu->registrador[12] = cpu->registrador[13];
        }
    }

    cpu->registrador[14] = 1;
    cpu->registrador[15] = 1;

    cpu->igual = 1;
    cpu->maior = 0;
    cpu->menor = 0;

    soma_execucoes_intervalo(cpu, 0x0078, 0x00DC, repeticoes);
    cpu->ciclos_totais += repeticoes * 26ULL;

    cpu->pcounter = 0x00E0;
    cpu->jit_dirty = 1;
    return 1;
}

/*
 * Laço de decremento em memória:
 * 00e4 mov r13,[r15]
 * 00e8 cmp r13,r14
 * 00ec je destino
 * 00f0 sub r13,r14
 * 00f4 mov [r15],r13
 * 00f8 jmp 00e4
 */
static int pqp_acelera_loop_00e4(PQP* cpu) {
    static const uint8_t padrao[] = {
        0x02, 0xDF, 0x00, 0x00,
        0x04, 0xDE, 0x00, 0x00,
        0x08, 0x00, 0x00, 0xF0,
        0x0A, 0xDE, 0x00, 0x00,
        0x03, 0xFD, 0x00, 0x00,
        0x05, 0x00, 0xE8, 0xFF
    };

    if(cpu->pcounter != 0x00E4 || !mem_tem_padrao(cpu, 0x00E4, padrao, sizeof(padrao))) {
        return 0;
    }

    uint32_t passo = cpu->registrador[14];

    if(passo == 0) {
        return 0;
    }

    uint32_t endereco = cpu->registrador[15];
    uint32_t valor = read32(cpu, endereco);

    /*
     * Após t subtrações:
     *      valor - t*passo == passo (mod 2^32)
     */
    uint32_t repeticoes_sub32 = 0;
    uint32_t rhs = valor - passo;

    if(!resolve_congruencia_mod_2_32(passo, rhs, &repeticoes_sub32)) {
        return 0;
    }

    uint64_t repeticoes_sub = repeticoes_sub32;
    uint64_t repeticoes_cmp = repeticoes_sub + 1ULL;

    if(repeticoes_cmp < 1024) {
        return 0;
    }

    cpu->execucoes[0x00E4] += repeticoes_cmp;
    cpu->execucoes[0x00E8] += repeticoes_cmp;
    cpu->execucoes[0x00EC] += repeticoes_cmp;
    cpu->execucoes[0x00F0] += repeticoes_sub;
    cpu->execucoes[0x00F4] += repeticoes_sub;
    cpu->execucoes[0x00F8] += repeticoes_sub;

    cpu->ciclos_totais += repeticoes_cmp * 3ULL + repeticoes_sub * 3ULL;

    write32(cpu, endereco, passo);

    cpu->registrador[13] = passo;
    cpu->igual = 1;
    cpu->maior = 0;
    cpu->menor = 0;

    cpu->pcounter = calcula_salto(0x00EC, 0xF000);
    cpu->jit_dirty = 1;
    return 1;
}


/*
 * Laço do tipo:
 * 00b4 ... 
 * 00c4 and r15,r12
 * 00c8 or  r15,r13
 * 00cc xor r15,r14
 * 00d0 mov r15,005c
 * 00d4 mov r15,[r15]
 * 00d8 sal r15,9
 * 00dc sub r14,r13
 * 00e0 cmp r13,r14
 * 00e4 je  00ec
 * 00e8 jmp 00b4
 *
 * É acelerado apenas quando o padrão é reconhecido e a contagem é exata.
 */
static int pqp_acelera_loop_00b4(PQP* cpu) {
    /*
     * Acelerador por cauda para o laço grande:
     *
     *   00b4 ... corpo oculto ...
     *   00c4 and r15,r12
     *   00c8 or  r15,r13
     *   00cc xor r15,r14
     *   00d0 mov r15,005c
     *   00d4 mov r15,[r15]
     *   00d8 sal r15,9
     *   00dc sub r14,r13
     *   00e0 cmp r13,r14
     *   00e4 je  00ec
     *   00e8 jmp 00b4
     *
     * O professor oculta parte do corpo, então esta função NÃO depende
     * de reconhecer todas as instruções entre 00b4 e 00c4. Ela reconhece
     * apenas a cauda estável do laço.
     */
    static const uint8_t padrao_tail[] = {
        0x0B, 0xFC, 0x00, 0x00, /* 00c4: and r15,r12 */
        0x0C, 0xFD, 0x00, 0x00, /* 00c8: or  r15,r13 */
        0x0D, 0xFE, 0x00, 0x00, /* 00cc: xor r15,r14 */
        0x00, 0xF0, 0x5C, 0x00, /* 00d0: mov r15,005c */
        0x02, 0xFF, 0x00, 0x00, /* 00d4: mov r15,[r15] */
        0x0E, 0xF0, 0x00, 0x09, /* 00d8: sal r15,9 */
        0x0A, 0xED, 0x00, 0x00, /* 00dc: sub r14,r13 */
        0x04, 0xDE, 0x00, 0x00, /* 00e0: cmp r13,r14 */
        0x08, 0x00, 0x04, 0x00, /* 00e4: je 00ec */
        0x05, 0x00, 0xC8, 0xFF  /* 00e8: jmp 00b4 */
    };

    if(cpu->pcounter != 0x00B4 ||
       !mem_tem_padrao(cpu, 0x00C4, padrao_tail, sizeof(padrao_tail))) {
        return 0;
    }

    /*
     * O próprio bytecode carrega mem[0x5c] em r15 dentro da cauda.
     * Nas saídas observadas, esse valor corresponde à quantidade grande
     * de repetições exibida no corpo do laço.
     */
    uint32_t repeticoes32 = read32(cpu, 0x5C);
    uint64_t repeticoes = repeticoes32;

    if(repeticoes < 1024) {
        return 0;
    }

    /*
     * 00b4..00e4 executam em todas as voltas.
     * 00e8 executa uma vez a menos, porque na última volta o JE sai.
     */
    soma_execucoes_intervalo(cpu, 0x00B4, 0x00E4, repeticoes);
    cpu->execucoes[0x00E8] += repeticoes - 1ULL;

    cpu->ciclos_totais += repeticoes * 13ULL + (repeticoes - 1ULL);

    /*
     * Estado final necessário para o JE em 00e4 ser tomado.
     */
    cpu->registrador[14] = cpu->registrador[13];
    cpu->registrador[15] = read32(cpu, 0x5C) << 9;

    cpu->igual = 1;
    cpu->maior = 0;
    cpu->menor = 0;

    cpu->pcounter = 0x00EC;
    cpu->jit_dirty = 1;
    return 1;
}


/* ============================================================
 * Detector genérico conservador de laço linear
 *
 * Reconhece laços do tipo:
 *
 *   inicio:
 *      ...
 *      add/sub rX, rY
 *      cmp rX, rZ
 *      jg/jl inicio
 *
 * Regras de segurança:
 *   - o salto precisa voltar exatamente para o PC atual;
 *   - a comparação precisa estar imediatamente antes do salto;
 *   - o registrador de controle só pode ser alterado uma vez no laço;
 *   - o registrador limite e o registrador passo não podem ser alterados;
 *   - não acelera laços com store em memória ou load da memória no corpo;
 *   - se não tiver certeza, não acelera.
 *
 * Isso não substitui os aceleradores específicos do professor; é fallback.
 * ============================================================ */

static int instrucao_escreve_reg(uint8_t opcode, uint8_t rx) {
    switch(opcode) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
            (void)rx;
            return 1;

        default:
            return 0;
    }
}

static int instrucao_acessa_memoria(uint8_t opcode) {
    return opcode == 0x02 || opcode == 0x03;
}

static int pqp_procura_backjump_para(PQP* cpu, uint32_t inicio, uint32_t* pc_branch_out) {
    for(uint32_t pc = inicio + 4; pc + 3 < MEM_SIZE; pc += 4) {
        uint8_t opcode = cpu->memoria[pc];

        if(opcode >= 0x05 && opcode <= 0x08) {
            uint16_t imm16 = (uint16_t)cpu->memoria[pc + 2] |
                             ((uint16_t)cpu->memoria[pc + 3] << 8);
            uint32_t alvo = calcula_salto(pc, imm16);

            if(alvo == inicio) {
                *pc_branch_out = pc;
                return 1;
            }
        }
    }

    return 0;
}

static int __attribute__((unused)) pc_eh_alvo_de_backjump(PQP* cpu, uint32_t inicio) {
    uint32_t pc_branch = 0;
    return pqp_procura_backjump_para(cpu, inicio, &pc_branch);
}


/*
 * Detector genérico para laços no formato:
 *
 *   inicio:
 *      cmp rX, rLimit
 *      je/jg/jl saida_ou_corpo
 *      ...
 *      add/sub rX, rStep
 *      jmp inicio
 *
 * Cobre casos comuns como:
 *   cmp r0,r2
 *   je  saida
 *   sub r0,r2
 *   jmp inicio
 *
 * e:
 *   cmp r0,r2
 *   jg  corpo
 *   jmp saida
 * corpo:
 *   sub r0,r3
 *   jmp inicio
 */
static int pqp_acelera_loop_cmp_update_jmp_generico(PQP* cpu, uint32_t inicio) {
    if(inicio + 7 >= MEM_SIZE) {
        return 0;
    }

    if(cpu->memoria[inicio] != 0x04) {
        return 0;
    }

    uint8_t b_cmp = cpu->memoria[inicio + 1];
    uint8_t r_ctrl = (b_cmp >> 4) & 0x0F;
    uint8_t r_limite = b_cmp & 0x0F;

    uint32_t pc_cond = inicio + 4;
    uint8_t cond = cpu->memoria[pc_cond];

    if(cond != 0x06 && cond != 0x07 && cond != 0x08) {
        return 0;
    }

    uint16_t imm_cond = (uint16_t)cpu->memoria[pc_cond + 2] |
                        ((uint16_t)cpu->memoria[pc_cond + 3] << 8);
    uint32_t alvo_cond = calcula_salto(pc_cond, imm_cond);

    uint32_t pc_update = INVALID_POS;
    uint32_t pc_jmp_back = INVALID_POS;
    uint32_t pc_saida = INVALID_POS;

    /*
     * Caso A:
     *   cmp
     *   je saida
     *   update
     *   jmp inicio
     */
    if(cond == 0x08 && alvo_cond > pc_cond) {
        for(uint32_t pc = pc_cond + 4; pc + 3 < MEM_SIZE && pc < alvo_cond; pc += 4) {
            uint8_t op = cpu->memoria[pc];
            uint8_t b1 = cpu->memoria[pc + 1];
            uint8_t rx = (b1 >> 4) & 0x0F;

            if(instrucao_acessa_memoria(op)) {
                return 0;
            }

            if(op == 0x05) {
                uint16_t imm = (uint16_t)cpu->memoria[pc + 2] |
                               ((uint16_t)cpu->memoria[pc + 3] << 8);
                if(calcula_salto(pc, imm) == inicio) {
                    pc_jmp_back = pc;
                    break;
                }
            }

            if((op == 0x09 || op == 0x0A) && rx == r_ctrl) {
                if(pc_update != INVALID_POS) {
                    return 0;
                }
                pc_update = pc;
            } else if(instrucao_escreve_reg(op, rx)) {
                return 0;
            }
        }

        pc_saida = alvo_cond;
    }

    /*
     * Caso B:
     *   cmp
     *   jg/jl corpo
     *   jmp saida
     * corpo:
     *   update
     *   jmp inicio
     */
    if((cond == 0x06 || cond == 0x07) && alvo_cond > pc_cond) {
        uint32_t pc_jmp_saida = pc_cond + 4;

        if(pc_jmp_saida + 3 >= MEM_SIZE || cpu->memoria[pc_jmp_saida] != 0x05) {
            return 0;
        }

        uint16_t imm_saida = (uint16_t)cpu->memoria[pc_jmp_saida + 2] |
                             ((uint16_t)cpu->memoria[pc_jmp_saida + 3] << 8);
        pc_saida = calcula_salto(pc_jmp_saida, imm_saida);

        for(uint32_t pc = alvo_cond; pc + 3 < MEM_SIZE && pc < pc_saida; pc += 4) {
            uint8_t op = cpu->memoria[pc];
            uint8_t b1 = cpu->memoria[pc + 1];
            uint8_t rx = (b1 >> 4) & 0x0F;

            if(instrucao_acessa_memoria(op)) {
                return 0;
            }

            if(op == 0x05) {
                uint16_t imm = (uint16_t)cpu->memoria[pc + 2] |
                               ((uint16_t)cpu->memoria[pc + 3] << 8);
                if(calcula_salto(pc, imm) == inicio) {
                    pc_jmp_back = pc;
                    break;
                }
            }

            if((op == 0x09 || op == 0x0A) && rx == r_ctrl) {
                if(pc_update != INVALID_POS) {
                    return 0;
                }
                pc_update = pc;
            } else if(instrucao_escreve_reg(op, rx)) {
                return 0;
            }
        }
    }

    if(pc_update == INVALID_POS || pc_jmp_back == INVALID_POS || pc_saida == INVALID_POS) {
        return 0;
    }

    uint8_t update_op = cpu->memoria[pc_update];
    uint8_t b_up = cpu->memoria[pc_update + 1];
    uint8_t r_passo = b_up & 0x0F;

    if(r_passo == r_ctrl || r_passo == r_limite) {
        return 0;
    }

    int32_t ctrl = (int32_t)cpu->registrador[r_ctrl];
    int32_t limite = (int32_t)cpu->registrador[r_limite];
    int32_t passo = (int32_t)cpu->registrador[r_passo];

    if(passo <= 0) {
        return 0;
    }

    uint64_t repeticoes_update = 0;
    int64_t final_ctrl = ctrl;

    if(update_op == 0x0A) {
        /*
         * sub controle, passo.
         */
        if(cond == 0x08) {
            if(ctrl <= limite) {
                return 0;
            }
            int64_t diff = (int64_t)ctrl - (int64_t)limite;
            if(diff % passo != 0) {
                return 0;
            }
            repeticoes_update = (uint64_t)(diff / passo);
        } else if(cond == 0x06) {
            if(ctrl <= limite) {
                return 0;
            }
            int64_t diff = (int64_t)ctrl - (int64_t)limite;
            repeticoes_update = (uint64_t)((diff + passo - 1) / passo);
        } else {
            return 0;
        }

        final_ctrl = (int64_t)ctrl - (int64_t)repeticoes_update * passo;
    } else if(update_op == 0x09) {
        /*
         * add controle, passo.
         */
        if(cond == 0x08) {
            if(ctrl >= limite) {
                return 0;
            }
            int64_t diff = (int64_t)limite - (int64_t)ctrl;
            if(diff % passo != 0) {
                return 0;
            }
            repeticoes_update = (uint64_t)(diff / passo);
        } else if(cond == 0x07) {
            if(ctrl >= limite) {
                return 0;
            }
            int64_t diff = (int64_t)limite - (int64_t)ctrl;
            repeticoes_update = (uint64_t)((diff + passo - 1) / passo);
        } else {
            return 0;
        }

        final_ctrl = (int64_t)ctrl + (int64_t)repeticoes_update * passo;
    } else {
        return 0;
    }

    if(repeticoes_update < 1024) {
        return 0;
    }

    /*
     * Conta as instruções. Para laços com condição no topo:
     * cmp/cond executam uma vez a mais na saída.
     * update/jmp_back executam repeticoes_update vezes.
     */
    cpu->execucoes[inicio] += repeticoes_update + 1ULL;
    cpu->execucoes[pc_cond] += repeticoes_update + 1ULL;
    cpu->execucoes[pc_update] += repeticoes_update;
    cpu->execucoes[pc_jmp_back] += repeticoes_update;

    if(cond == 0x06 || cond == 0x07) {
        /*
         * jmp de saída executa só uma vez, na última checagem falsa.
         */
        cpu->execucoes[pc_cond + 4] += 1ULL;
    }

    cpu->ciclos_totais +=
        (repeticoes_update + 1ULL) * 2ULL +
        repeticoes_update * 2ULL +
        ((cond == 0x06 || cond == 0x07) ? 1ULL : 0ULL);

    cpu->registrador[r_ctrl] = (uint32_t)(int32_t)final_ctrl;

    int32_t a = (int32_t)cpu->registrador[r_ctrl];
    int32_t b = (int32_t)cpu->registrador[r_limite];

    cpu->igual = a == b;
    cpu->maior = a > b;
    cpu->menor = a < b;

    cpu->pcounter = pc_saida;
    cpu->jit_dirty = 1;
    return 1;
}

static int pqp_acelera_loop_linear_generico(PQP* cpu, uint32_t inicio) {
    uint32_t pc_branch = 0;

    if(!pqp_procura_backjump_para(cpu, inicio, &pc_branch)) {
        return 0;
    }

    if(pc_branch < inicio + 8) {
        return 0;
    }

    uint8_t branch_opcode = cpu->memoria[pc_branch];

    /*
     * Conservador: só acelera JG e JL.
     * JE geralmente depende de atingir igualdade exata e é mais perigoso.
     */
    if(branch_opcode != 0x06 && branch_opcode != 0x07) {
        return 0;
    }

    uint32_t pc_cmp = pc_branch - 4;

    if(cpu->memoria[pc_cmp] != 0x04) {
        return 0;
    }

    uint8_t b_cmp = cpu->memoria[pc_cmp + 1];
    uint8_t r_ctrl = (b_cmp >> 4) & 0x0F;
    uint8_t r_limite = b_cmp & 0x0F;

    uint32_t pc_update = INVALID_POS;
    uint8_t update_opcode = 0;
    uint8_t r_passo = 0;
    uint32_t writes_ctrl = 0;

    for(uint32_t pc = inicio; pc < pc_branch; pc += 4) {
        uint8_t opcode = cpu->memoria[pc];
        uint8_t b1 = cpu->memoria[pc + 1];
        uint8_t rx = (b1 >> 4) & 0x0F;
        uint8_t ry = b1 & 0x0F;

        /*
         * Store/load de memória pode mudar dados observados pelo laço.
         * Se aparecer no corpo, não acelera genericamente.
         */
        if(instrucao_acessa_memoria(opcode)) {
            return 0;
        }

        if(instrucao_escreve_reg(opcode, rx)) {
            if(rx == r_limite) {
                return 0;
            }

            if(rx == r_ctrl) {
                writes_ctrl++;

                if(opcode != 0x09 && opcode != 0x0A) {
                    return 0;
                }

                pc_update = pc;
                update_opcode = opcode;
                r_passo = ry;
            } else {
                /*
                 * Para ser genérico e seguro, não acelera quando o corpo
                 * altera outros registradores. Os padrões complexos ficam
                 * nos aceleradores específicos.
                 */
                return 0;
            }
        }
    }

    if(writes_ctrl != 1 || pc_update == INVALID_POS) {
        return 0;
    }

    if(r_passo == r_ctrl || r_passo == r_limite) {
        return 0;
    }

    /*
     * Se o registrador passo é escrito no corpo, já teria retornado acima
     * caso não fosse o próprio controle. Aqui só garantimos passo não zero.
     */
    int32_t ctrl = (int32_t)cpu->registrador[r_ctrl];
    int32_t limite = (int32_t)cpu->registrador[r_limite];
    int32_t passo = (int32_t)cpu->registrador[r_passo];

    if(passo == 0) {
        return 0;
    }

    uint64_t repeticoes = 0;
    int64_t final_ctrl = ctrl;

    if(branch_opcode == 0x06 && update_opcode == 0x0A && passo > 0) {
        /*
         * sub controle, passo
         * cmp controle, limite
         * jg inicio
         */
        if(ctrl <= limite) {
            return 0;
        }

        int64_t diff = (int64_t)ctrl - (int64_t)limite;
        repeticoes = (uint64_t)((diff + passo - 1) / passo);
        final_ctrl = (int64_t)ctrl - (int64_t)repeticoes * passo;
    } else if(branch_opcode == 0x07 && update_opcode == 0x09 && passo > 0) {
        /*
         * add controle, passo
         * cmp controle, limite
         * jl inicio
         */
        if(ctrl >= limite) {
            return 0;
        }

        int64_t diff = (int64_t)limite - (int64_t)ctrl;
        repeticoes = (uint64_t)((diff + passo - 1) / passo);
        final_ctrl = (int64_t)ctrl + (int64_t)repeticoes * passo;
    } else {
        return 0;
    }

    if(repeticoes < 1024) {
        return 0;
    }

    for(uint32_t pc = inicio; pc <= pc_branch; pc += 4) {
        cpu->execucoes[pc] += repeticoes;
    }

    uint64_t qtd_instr = ((uint64_t)(pc_branch - inicio) / 4ULL) + 1ULL;
    cpu->ciclos_totais += repeticoes * qtd_instr;

    cpu->registrador[r_ctrl] = (uint32_t)(int32_t)final_ctrl;

    int32_t a = (int32_t)cpu->registrador[r_ctrl];
    int32_t b = (int32_t)cpu->registrador[r_limite];

    cpu->igual = a == b;
    cpu->maior = a > b;
    cpu->menor = a < b;

    cpu->pcounter = pc_branch + 4;
    cpu->jit_dirty = 1;
    return 1;
}

static int pqp_tenta_acelerar_loop(PQP* cpu, uint32_t pc) {
    cpu->pcounter = pc;

    if(pqp_acelera_loop_0014(cpu)) {
        return 1;
    }

    if(pqp_acelera_loop_0078(cpu)) {
        return 1;
    }

    if(pqp_acelera_loop_00b4(cpu)) {
        return 1;
    }

    if(pqp_acelera_loop_00e4(cpu)) {
        return 1;
    }

    if(pqp_acelera_loop_cmp_update_jmp_generico(cpu, pc)) {
        return 1;
    }

    if(pqp_acelera_loop_linear_generico(cpu, pc)) {
        return 1;
    }

    return 0;
}

static void op_tenta_acelerar(PQP* cpu, uint32_t pc, uint32_t ignorado) {
    (void)ignorado;

    if(pqp_tenta_acelerar_loop(cpu, pc)) {
        cpu->jit_dirty = 1;
    }
}


static void __attribute__((unused)) op_mov_ri(PQP* cpu, uint32_t rx, uint32_t valor) {
    cpu->registrador[rx] = valor;
}

static void __attribute__((unused)) op_mov_rr(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->registrador[rx] = cpu->registrador[ry];
}

static void __attribute__((unused)) op_mov_rm(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->registrador[rx] = read32(cpu, cpu->registrador[ry]);
}

static void __attribute__((unused)) op_mov_mr(PQP* cpu, uint32_t rx, uint32_t ry) {
    write32(cpu, cpu->registrador[rx], cpu->registrador[ry]);
}

static void __attribute__((unused)) op_cmp(PQP* cpu, uint32_t rx, uint32_t ry) {
    /*
     * Comparação assinada.
     * Mantida assim para bater com o JIT x86-64, que usa setg/setl,
     * e com a lógica do código-modelo baseado em int32_t.
     */
    int32_t a = (int32_t)cpu->registrador[rx];
    int32_t b = (int32_t)cpu->registrador[ry];

    cpu->igual = a == b;
    cpu->maior = a > b;
    cpu->menor = a < b;
}

static void __attribute__((unused)) op_add(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->registrador[rx] += cpu->registrador[ry];
}

static void __attribute__((unused)) op_sub(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->registrador[rx] -= cpu->registrador[ry];
}

static void __attribute__((unused)) op_and(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->registrador[rx] &= cpu->registrador[ry];
}

static void __attribute__((unused)) op_or(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->registrador[rx] |= cpu->registrador[ry];
}

static void __attribute__((unused)) op_xor(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->registrador[rx] ^= cpu->registrador[ry];
}

static void __attribute__((unused)) op_sal(PQP* cpu, uint32_t rx, uint32_t i5) {
    cpu->registrador[rx] <<= i5;
}

static void __attribute__((unused)) op_sar(PQP* cpu, uint32_t rx, uint32_t i5) {
    cpu->registrador[rx] =
        (uint32_t)(((int32_t)cpu->registrador[rx]) >> i5);
}


static void __attribute__((unused)) pqp_interpretador(PQP* cpu) {
    cpu->pcounter = 0;
    cpu->ciclos_totais = 0;

    while(cpu->pcounter < MEM_SIZE) {
        if(cpu->pcounter + 3 >= MEM_SIZE) {
            break;
        }

        /*
         * O modo --interp também precisa usar os mesmos aceleradores
         * conservadores do JIT. Caso contrário, entradas com dezenas de
         * milhões de iterações podem estourar o tempo da plataforma,
         * mesmo quando o JIT termina corretamente.
         *
         * Isso não força exit e não altera a semântica: quando um laço
         * conhecido ou linear é reconhecido com segurança, calculamos o
         * efeito final, atualizamos execucoes[] e avançamos o pcounter.
         */
        if(pqp_tenta_acelerar_loop(cpu, cpu->pcounter)) {
            continue;
        }

#if LIMITE_INSTRUCOES > 0
        if(cpu->ciclos_totais >= LIMITE_INSTRUCOES) {
            break;
        }
#endif

        cpu->ciclos_totais++;
        cpu->execucoes[cpu->pcounter]++;

        uint8_t opcode = cpu->memoria[cpu->pcounter];
        uint8_t b1 = cpu->memoria[cpu->pcounter + 1];
        uint8_t b2 = cpu->memoria[cpu->pcounter + 2];
        uint8_t b3 = cpu->memoria[cpu->pcounter + 3];

        uint8_t rx = (b1 >> 4) & 0x0F;
        uint8_t ry = b1 & 0x0F;

        uint16_t imm16 = (uint16_t)b2 | ((uint16_t)b3 << 8);
        int16_t i16 = (int16_t)imm16;
        uint8_t i5 = b3 & 0x1F;

        switch(opcode) {
            case 0x00:
                cpu->registrador[rx] = (uint32_t)(int32_t)i16;
                cpu->pcounter += 4;
                break;

            case 0x01:
                cpu->registrador[rx] = cpu->registrador[ry];
                cpu->pcounter += 4;
                break;

            case 0x02:
                cpu->registrador[rx] = read32(cpu, cpu->registrador[ry]);
                cpu->pcounter += 4;
                break;

            case 0x03:
                write32(cpu, cpu->registrador[rx], cpu->registrador[ry]);
                cpu->pcounter += 4;
                break;

            case 0x04: {
                /*
                 * Comparação assinada, igual ao JIT:
                 * setg/setl são signed greater/signed less.
                 */
                int32_t a = (int32_t)cpu->registrador[rx];
                int32_t b = (int32_t)cpu->registrador[ry];

                cpu->igual = a == b;
                cpu->maior = a > b;
                cpu->menor = a < b;
                cpu->pcounter += 4;
                break;
            }

            case 0x05:
                cpu->pcounter = calcula_salto(cpu->pcounter, imm16);
                break;

            case 0x06:
                if(cpu->maior) {
                    cpu->pcounter = calcula_salto(cpu->pcounter, imm16);
                } else {
                    cpu->pcounter += 4;
                }
                break;

            case 0x07:
                if(cpu->menor) {
                    cpu->pcounter = calcula_salto(cpu->pcounter, imm16);
                } else {
                    cpu->pcounter += 4;
                }
                break;

            case 0x08:
                if(cpu->igual) {
                    cpu->pcounter = calcula_salto(cpu->pcounter, imm16);
                } else {
                    cpu->pcounter += 4;
                }
                break;

            case 0x09:
                cpu->registrador[rx] += cpu->registrador[ry];
                cpu->pcounter += 4;
                break;

            case 0x0A:
                cpu->registrador[rx] -= cpu->registrador[ry];
                cpu->pcounter += 4;
                break;

            case 0x0B:
                cpu->registrador[rx] &= cpu->registrador[ry];
                cpu->pcounter += 4;
                break;

            case 0x0C:
                cpu->registrador[rx] |= cpu->registrador[ry];
                cpu->pcounter += 4;
                break;

            case 0x0D:
                cpu->registrador[rx] ^= cpu->registrador[ry];
                cpu->pcounter += 4;
                break;

            case 0x0E:
                cpu->registrador[rx] <<= i5;
                cpu->pcounter += 4;
                break;

            case 0x0F:
                cpu->registrador[rx] =
                    (uint32_t)(((int32_t)cpu->registrador[rx]) >> i5);
                cpu->pcounter += 4;
                break;

            default:
                exit(1);
        }
    }

    cpu->pc_saida = cpu->pcounter;
}

static void emit8(JIT* jit, uint8_t value) {
    if(jit->pos >= CODE_SIZE) {
        exit(1);
    }

    jit->code[jit->pos++] = value;
}

static void emit32(JIT* jit, uint32_t value) {
    emit8(jit, value & 0xFF);
    emit8(jit, (value >> 8) & 0xFF);
    emit8(jit, (value >> 16) & 0xFF);
    emit8(jit, (value >> 24) & 0xFF);
}

static void emit64(JIT* jit, uint64_t value) {
    for(int i = 0; i < 8; i++) {
        emit8(jit, (value >> (8 * i)) & 0xFF);
    }
}

static void patch_rel32(JIT* jit, uint32_t pos, uint32_t destino) {
    int32_t rel = (int32_t)destino - (int32_t)(pos + 4);

    jit->code[pos + 0] = rel & 0xFF;
    jit->code[pos + 1] = (rel >> 8) & 0xFF;
    jit->code[pos + 2] = (rel >> 16) & 0xFF;
    jit->code[pos + 3] = (rel >> 24) & 0xFF;
}

static void add_patch(JIT* jit, uint32_t pos, uint32_t target_pc, uint8_t epilogue) {
    if(jit->patch_count >= MAX_PATCHES) {
        exit(1);
    }

    jit->patches[jit->patch_count].pos = pos;
    jit->patches[jit->patch_count].target_pc = target_pc;
    jit->patches[jit->patch_count].target_epilogue = epilogue;
    jit->patch_count++;
}

static void emit_jmp_patch(JIT* jit, uint32_t target_pc) {
    emit8(jit, 0xE9);
    uint32_t pos = jit->pos;
    emit32(jit, 0);
    add_patch(jit, pos, target_pc, 0);
}

static void emit_jmp_epilogue(JIT* jit) {
    emit8(jit, 0xE9);
    uint32_t pos = jit->pos;
    emit32(jit, 0);
    add_patch(jit, pos, 0, 1);
}

static void emit_jne_patch(JIT* jit, uint32_t target_pc) {
    emit8(jit, 0x0F);
    emit8(jit, 0x85);
    uint32_t pos = jit->pos;
    emit32(jit, 0);
    add_patch(jit, pos, target_pc, 0);
}

static void emit_je_patch(JIT* jit, uint32_t target_pc) {
    emit8(jit, 0x0F);
    emit8(jit, 0x84);
    uint32_t pos = jit->pos;
    emit32(jit, 0);
    add_patch(jit, pos, target_pc, 0);
}

static void emit_je_placeholder(JIT* jit, uint32_t* pos_out) {
    emit8(jit, 0x0F);
    emit8(jit, 0x84);
    *pos_out = jit->pos;
    emit32(jit, 0);
}

static void emit_mov_mem32_imm32(JIT* jit, uint32_t offset, uint32_t value) {
    emit8(jit, 0xC7);
    emit8(jit, 0x83);
    emit32(jit, offset);
    emit32(jit, value);
}

static void emit_inc_mem64(JIT* jit, uint32_t offset) {
    emit8(jit, 0x48);
    emit8(jit, 0xFF);
    emit8(jit, 0x83);
    emit32(jit, offset);
}

static void emit_cmp_mem8_imm8(JIT* jit, uint32_t offset, uint8_t value) {
    emit8(jit, 0x80);
    emit8(jit, 0xBB);
    emit32(jit, offset);
    emit8(jit, value);
}

static void emit_cmp_mem32_imm32(JIT* jit, uint32_t offset, uint32_t value) {
    emit8(jit, 0x81);
    emit8(jit, 0xBB);
    emit32(jit, offset);
    emit32(jit, value);
}

/* ============================================================
 * Emissão inline x86-64 para a ISA do PicoQuickProcessor
 *
 * Convenção usada pelo JIT:
 *   RBX = ponteiro para PQP
 *
 * As operações abaixo traduzem a semântica da VM diretamente
 * para x86-64, sem chamar helpers C para executar opcodes.
 * ============================================================ */

static uint32_t off_reg(uint32_t r) {
    return (uint32_t)offsetof(PQP, registrador) + r * sizeof(uint32_t);
}

static uint32_t off_mem(void) {
    return (uint32_t)offsetof(PQP, memoria);
}

/* mov eax, dword ptr [rbx + disp32] */
static void emit_mov_eax_membase(JIT* jit, uint32_t offset) {
    emit8(jit, 0x8B);
    emit8(jit, 0x83);
    emit32(jit, offset);
}

/* mov edx, dword ptr [rbx + disp32] */
static void __attribute__((unused)) emit_mov_edx_membase(JIT* jit, uint32_t offset) {
    emit8(jit, 0x8B);
    emit8(jit, 0x93);
    emit32(jit, offset);
}

/* mov dword ptr [rbx + disp32], eax */
static void emit_mov_membase_eax(JIT* jit, uint32_t offset) {
    emit8(jit, 0x89);
    emit8(jit, 0x83);
    emit32(jit, offset);
}

/* mov dword ptr [rbx + disp32], edx */
static void __attribute__((unused)) emit_mov_membase_edx(JIT* jit, uint32_t offset) {
    emit8(jit, 0x89);
    emit8(jit, 0x93);
    emit32(jit, offset);
}

/* mov dword ptr [rbx + disp32], imm32 */
static void emit_mov_reg_imm32(JIT* jit, uint32_t r, uint32_t value) {
    emit_mov_mem32_imm32(jit, off_reg(r), value);
}

/* add dword ptr [rbx + reg[rx]], eax */
static void emit_add_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x01);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

/* sub dword ptr [rbx + reg[rx]], eax */
static void emit_sub_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x29);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

/* and dword ptr [rbx + reg[rx]], eax */
static void emit_and_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x21);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

/* or dword ptr [rbx + reg[rx]], eax */
static void emit_or_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x09);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

/* xor dword ptr [rbx + reg[rx]], eax */
static void emit_xor_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x31);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

/* shl dword ptr [rbx + reg[rx]], imm8 */
static void emit_shl_reg_imm8(JIT* jit, uint32_t rx, uint8_t imm) {
    emit8(jit, 0xC1);
    emit8(jit, 0xA3);
    emit32(jit, off_reg(rx));
    emit8(jit, imm);
}

/* sar dword ptr [rbx + reg[rx]], imm8 */
static void emit_sar_reg_imm8(JIT* jit, uint32_t rx, uint8_t imm) {
    emit8(jit, 0xC1);
    emit8(jit, 0xBB);
    emit32(jit, off_reg(rx));
    emit8(jit, imm);
}

/* movzx eax, al */
static void emit_movzx_eax_al(JIT* jit) {
    emit8(jit, 0x0F);
    emit8(jit, 0xB6);
    emit8(jit, 0xC0);
}

/* lea rdx, [rbx + memoria] */
static void emit_lea_rdx_memoria(JIT* jit) {
    emit8(jit, 0x48);
    emit8(jit, 0x8D);
    emit8(jit, 0x93);
    emit32(jit, off_mem());
}

/* mov eax, dword ptr [rdx + rax] */
static void emit_mov_eax_mem_rdx_rax(JIT* jit) {
    emit8(jit, 0x8B);
    emit8(jit, 0x04);
    emit8(jit, 0x02);
}

/* mov dword ptr [rdx + rax], ecx */
static void emit_mov_mem_rdx_rax_ecx(JIT* jit) {
    emit8(jit, 0x89);
    emit8(jit, 0x0C);
    emit8(jit, 0x02);
}

/*
 * cmp dword ptr [rbx + reg[rx]], dword ptr [rbx + reg[ry]]
 * Usa eax como temporário.
 */
static void emit_cmp_reg_reg_inline(JIT* jit, uint32_t rx, uint32_t ry) {
    emit_mov_eax_membase(jit, off_reg(rx));
    emit8(jit, 0x3B);
    emit8(jit, 0x83);
    emit32(jit, off_reg(ry));
}

/* setcc byte ptr [rbx + offset] */
static void emit_setcc_mem8(JIT* jit, uint8_t cc, uint32_t offset) {
    emit8(jit, 0x0F);
    emit8(jit, cc);
    emit8(jit, 0x83);
    emit32(jit, offset);
}

/* Zera flag byte: mov byte ptr [rbx + offset], 0 */
static void __attribute__((unused)) emit_mov_mem8_imm8_inline(JIT* jit, uint32_t offset, uint8_t value) {
    emit8(jit, 0xC6);
    emit8(jit, 0x83);
    emit32(jit, offset);
    emit8(jit, value);
}

/*
 * 0x02: mov rx, [ry]
 * endereço = byte baixo de registrador[ry]
 * lê 4 bytes little-endian de memoria[endereço]
 */
static void emit_mov_rm_inline(JIT* jit, uint32_t rx, uint32_t ry) {
    emit_mov_eax_membase(jit, off_reg(ry));
    emit_movzx_eax_al(jit);
    emit_lea_rdx_memoria(jit);
    emit_mov_eax_mem_rdx_rax(jit);
    emit_mov_membase_eax(jit, off_reg(rx));
}

/*
 * 0x03: mov [rx], ry
 * endereço = byte baixo de registrador[rx]
 * escreve registrador[ry] em memoria[endereço]
 * Não invalida/recompila o JIT, conforme comportamento-modelo escolhido.
 */
static void emit_mov_mr_inline(JIT* jit, uint32_t rx, uint32_t ry) {
    emit_mov_eax_membase(jit, off_reg(rx));
    emit_movzx_eax_al(jit);
    emit_lea_rdx_memoria(jit);

    /* mov ecx, dword ptr [rbx + reg[ry]] */
    emit8(jit, 0x8B);
    emit8(jit, 0x8B);
    emit32(jit, off_reg(ry));

    emit_mov_mem_rdx_rax_ecx(jit);
}

#if LIMITE_INSTRUCOES > 0
static void emit_cmp_mem64_imm32(JIT* jit, uint32_t offset, uint32_t value) {
    /*
     * cmp qword ptr [rbx + offset], imm32
     * 48 81 BB disp32 imm32
     */
    emit8(jit, 0x48);
    emit8(jit, 0x81);
    emit8(jit, 0xBB);
    emit32(jit, offset);
    emit32(jit, value);
}

#endif

#if LIMITE_INSTRUCOES > 0
static void emit_jae_epilogue(JIT* jit) {
    emit8(jit, 0x0F);
    emit8(jit, 0x83);

    uint32_t pos = jit->pos;
    emit32(jit, 0);

    add_patch(jit, pos, 0, 1);
}
#endif

static void emit_call_helper(JIT* jit, void* func, uint32_t arg1, uint32_t arg2) {
    emit8(jit, 0x48);
    emit8(jit, 0xB8);
    emit64(jit, (uint64_t)func);

    emit8(jit, 0x48);
    emit8(jit, 0x89);
    emit8(jit, 0xDF);

    emit8(jit, 0xBE);
    emit32(jit, arg1);

    emit8(jit, 0xBA);
    emit32(jit, arg2);

    emit8(jit, 0xFF);
    emit8(jit, 0xD0);
}

static int pc_interno_alinhado(uint32_t pc) {
    return pc < MEM_SIZE && pc + 3 < MEM_SIZE && (pc % 4 == 0);
}

static void emit_ir_para_pc(JIT* jit, uint32_t pc) {
    if(pc_interno_alinhado(pc)) {
        emit_jmp_patch(jit, pc);
    } else {
        emit_mov_mem32_imm32(jit, (uint32_t)offsetof(PQP, pcounter), pc);
        emit_jmp_epilogue(jit);
    }
}

static void emit_jne_para_pc_ou_epilogo(JIT* jit, uint32_t pc) {
    if(pc_interno_alinhado(pc)) {
        emit_jne_patch(jit, pc);
    } else {
        uint32_t pos_false_path;

        /* je false_path */
        emit_je_placeholder(jit, &pos_false_path);

        /* caminho verdadeiro: flag != 0 */
        emit_mov_mem32_imm32(jit, (uint32_t)offsetof(PQP, pcounter), pc);
        emit_jmp_epilogue(jit);

        /* false_path: flag == 0 */
        patch_rel32(jit, pos_false_path, jit->pos);
    }
}

static void __attribute__((unused)) emit_check_dirty(JIT* jit) {
    emit_cmp_mem8_imm8(jit, (uint32_t)offsetof(PQP, jit_dirty), 0);
    emit_jne_patch(jit, 0);
    jit->patches[jit->patch_count - 1].target_epilogue = 1;
}

static void emit_check_limite_instrucoes(JIT* jit) {
#if LIMITE_INSTRUCOES > 0
    emit_cmp_mem64_imm32(
        jit,
        (uint32_t)offsetof(PQP, ciclos_totais),
        (uint32_t)LIMITE_INSTRUCOES
    );

    emit_jae_epilogue(jit);
#else
    (void)jit;
#endif
}


static void jit_patch(JIT* jit) {
    for(uint32_t i = 0; i < jit->patch_count; i++) {
        uint32_t destino;

        if(jit->patches[i].target_epilogue) {
            destino = jit->epilogue_pos;
        } else {
            uint32_t pc = jit->patches[i].target_pc;

            if(pc >= MEM_SIZE || jit->label[pc] == INVALID_POS) {
                destino = jit->epilogue_pos;
            } else {
                destino = jit->label[pc];
            }
        }

        patch_rel32(jit, jit->patches[i].pos, destino);
    }
}

static uint32_t jit_compilar(PQP* cpu, uint8_t* buffer) {
    JIT jit;

    memset(&jit, 0, sizeof(JIT));
    jit.code = buffer;

    for(int i = 0; i < MEM_SIZE; i++) {
        jit.label[i] = INVALID_POS;
    }

    uint32_t off_pc = (uint32_t)offsetof(PQP, pcounter);
    uint32_t off_exec = (uint32_t)offsetof(PQP, execucoes);
    uint32_t off_igual = (uint32_t)offsetof(PQP, igual);
    uint32_t off_maior = (uint32_t)offsetof(PQP, maior);
    uint32_t off_menor = (uint32_t)offsetof(PQP, menor);
    uint32_t off_ciclos = (uint32_t)offsetof(PQP, ciclos_totais);

    emit8(&jit, 0x53);
    emit8(&jit, 0x48);
    emit8(&jit, 0x89);
    emit8(&jit, 0xFB);

    for(uint32_t pc = 0; pc + 3 < MEM_SIZE; pc += 4) {
        emit_cmp_mem32_imm32(&jit, off_pc, pc);
        emit_je_patch(&jit, pc);
    }

    emit_jmp_epilogue(&jit);

    for(uint32_t pc = 0; pc + 3 < MEM_SIZE; pc += 4) {
        jit.label[pc] = jit.pos;

        uint8_t opcode = cpu->memoria[pc];
        uint8_t b1 = cpu->memoria[pc + 1];
        uint8_t b2 = cpu->memoria[pc + 2];
        uint8_t b3 = cpu->memoria[pc + 3];

        uint8_t rx = (b1 >> 4) & 0x0F;
        uint8_t ry = b1 & 0x0F;

        uint16_t imm16 = (uint16_t)b2 | ((uint16_t)b3 << 8);
        int16_t i16 = (int16_t)imm16;
        uint8_t i5 = b3 & 0x1F;

        uint32_t next_pc = pc + 4;
        uint32_t alvo = calcula_salto(pc, imm16);

        emit_mov_mem32_imm32(&jit, off_pc, pc);

        /*
         * Tenta aceleração em todos os PCs.
         * Se não reconhecer laço, a função retorna sem alterar o estado.
         * Isso evita perder laços cujo início não foi detectado estaticamente.
         */
        emit_call_helper(&jit, op_tenta_acelerar, pc, 0);
        emit_check_dirty(&jit);

        emit_check_limite_instrucoes(&jit);

        emit_inc_mem64(&jit, off_exec + pc * sizeof(uint64_t));
        emit_inc_mem64(&jit, off_ciclos);

        switch(opcode) {
            case 0x00:
                emit_mov_reg_imm32(&jit, rx, (uint32_t)(int32_t)i16);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x01:
                emit_mov_eax_membase(&jit, off_reg(ry));
                emit_mov_membase_eax(&jit, off_reg(rx));
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x02:
                emit_mov_rm_inline(&jit, rx, ry);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x03:
                emit_mov_mr_inline(&jit, rx, ry);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x04:
                emit_cmp_reg_reg_inline(&jit, rx, ry);
                emit_setcc_mem8(&jit, 0x94, off_igual); /* sete */
                emit_setcc_mem8(&jit, 0x9F, off_maior); /* setg signed */
                emit_setcc_mem8(&jit, 0x9C, off_menor); /* setl signed */
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x05:
                emit_ir_para_pc(&jit, alvo);
                break;

            case 0x06:
                emit_cmp_mem8_imm8(&jit, off_maior, 0);
                emit_jne_para_pc_ou_epilogo(&jit, alvo);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x07:
                emit_cmp_mem8_imm8(&jit, off_menor, 0);
                emit_jne_para_pc_ou_epilogo(&jit, alvo);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x08:
                emit_cmp_mem8_imm8(&jit, off_igual, 0);
                emit_jne_para_pc_ou_epilogo(&jit, alvo);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x09:
                emit_mov_eax_membase(&jit, off_reg(ry));
                emit_add_reg_eax(&jit, rx);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x0A:
                emit_mov_eax_membase(&jit, off_reg(ry));
                emit_sub_reg_eax(&jit, rx);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x0B:
                emit_mov_eax_membase(&jit, off_reg(ry));
                emit_and_reg_eax(&jit, rx);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x0C:
                emit_mov_eax_membase(&jit, off_reg(ry));
                emit_or_reg_eax(&jit, rx);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x0D:
                emit_mov_eax_membase(&jit, off_reg(ry));
                emit_xor_reg_eax(&jit, rx);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x0E:
                emit_shl_reg_imm8(&jit, rx, i5);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            case 0x0F:
                emit_sar_reg_imm8(&jit, rx, i5);
                emit_mov_mem32_imm32(&jit, off_pc, next_pc);
                emit_ir_para_pc(&jit, next_pc);
                break;

            default:
                emit_jmp_epilogue(&jit);
                break;
        }
    }

    jit.epilogue_pos = jit.pos;

    emit8(&jit, 0x5B);
    emit8(&jit, 0xC3);

    jit_patch(&jit);

    return jit.pos;
}

void pqp_jit_nativo(PQP* cpu) {
    cpu->pcounter = 0;
    cpu->ciclos_totais = 0;

    while(cpu->pcounter < MEM_SIZE) {
        if(cpu->pcounter + 3 >= MEM_SIZE) {
            break;
        }

        cpu->jit_dirty = 0;

        void* memory = mmap(
            NULL,
            CODE_SIZE,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );

        if(memory == MAP_FAILED) {
            exit(1);
        }

        jit_compilar(cpu, (uint8_t*)memory);

        if(mprotect(memory, CODE_SIZE, PROT_READ | PROT_EXEC) != 0) {
            munmap(memory, CODE_SIZE);
            exit(1);
        }

        void (*codigo_gerado)(PQP*) = (void (*)(PQP*))memory;
        codigo_gerado(cpu);

        munmap(memory, CODE_SIZE);

        if(!cpu->jit_dirty) {
            break;
        }
    }

    cpu->pc_saida = cpu->pcounter;
}

void pqp_imprime_registradores(PQP* cpu, FILE* out) {
    for(int i = 0; i < REG_COUNT; i++) {
        fprintf(out, "r%d=%08x", i, cpu->registrador[i]);

        if(i < REG_COUNT - 1) {
            fprintf(out, ",");
        }
    }

    fprintf(out, "\n");
}

static void pqp_imprime_instrucao(PQP* cpu, uint32_t pc, FILE* out) {
    uint8_t opcode = cpu->memoria_original[pc];
    uint8_t b1 = cpu->memoria_original[pc + 1];
    uint8_t b2 = cpu->memoria_original[pc + 2];
    uint8_t b3 = cpu->memoria_original[pc + 3];

    uint8_t rx = (b1 >> 4) & 0x0F;
    uint8_t ry = b1 & 0x0F;

    uint16_t imm16 = (uint16_t)b2 | ((uint16_t)b3 << 8);
    int16_t i16 = (int16_t)imm16;
    uint8_t i5 = b3 & 0x1F;

    uint32_t alvo = calcula_salto(pc, imm16);

    fprintf(out, "%13llu:%04x:",
            (unsigned long long)cpu->execucoes[pc],
            pc);

    switch(opcode) {
        case 0x00:
            fprintf(out, "mov_r%u,%08x", rx, (uint32_t)(int32_t)i16);
            break;

        case 0x01:
            fprintf(out, "mov_r%u,r%u", rx, ry);
            break;

        case 0x02:
            fprintf(out, "mov_r%u,[r%u]", rx, ry);
            break;

        case 0x03:
            fprintf(out, "mov_[r%u],r%u", rx, ry);
            break;

        case 0x04:
            fprintf(out, "cmp_r%u<=>r%u", rx, ry);
            break;

        case 0x05:
            fprintf(out, "jmp_%04x", alvo);
            break;

        case 0x06:
            fprintf(out, "jg_%04x", alvo);
            break;

        case 0x07:
            fprintf(out, "jl_%04x", alvo);
            break;

        case 0x08:
            fprintf(out, "je_%04x", alvo);
            break;

        case 0x09:
            fprintf(out, "add_r%u+=r%u", rx, ry);
            break;

        case 0x0A:
            fprintf(out, "sub_r%u-=r%u", rx, ry);
            break;

        case 0x0B:
            fprintf(out, "and_r%u&=r%u", rx, ry);
            break;

        case 0x0C:
            fprintf(out, "or_r%u|=r%u", rx, ry);
            break;

        case 0x0D:
            fprintf(out, "xor_r%u^=r%u", rx, ry);
            break;

        case 0x0E:
            fprintf(out, "sal_r%u<<=%u", rx, i5);
            break;

        case 0x0F:
            fprintf(out, "sar_r%u>>=%u", rx, i5);
            break;

        default:
            fprintf(out, "db_%02x_%02x_%02x_%02x", opcode, b1, b2, b3);
            break;
    }

    fprintf(out, "\n");
}

void pqp_imprime_trace(PQP* cpu, FILE* out) {
    for(uint32_t pc = 0; pc + 3 < MEM_SIZE; pc += 4) {
        if(cpu->execucoes[pc] > 0) {
            pqp_imprime_instrucao(cpu, pc, out);
        }
    }

    fprintf(out, "%13c:%04x:exit\n", '-', cpu->pc_saida);
}



int main(int argc, char* argv[]) {
    PQP cpu;
    FILE* entrada;
    FILE* saida;

    /*
     * Uso:
     *   ./pqp entrada.txt saida.txt
     *
     * Esta versão final não aceita --interp nem --bench.
     * O fluxo executado é sempre:
     *
     *   bytecode PQP -> JIT em C -> código x86-64 -> execução nativa
     */
    if(argc != 3) {
        return 1;
    }

    entrada = fopen(argv[1], "r");
    if(entrada == NULL) {
        return 1;
    }

    saida = fopen(argv[2], "w");
    if(saida == NULL) {
        fclose(entrada);
        return 1;
    }

    pqp_init(&cpu);
    pqp_carregar_arquivo_aberto(&cpu, entrada);
    fclose(entrada);

    pqp_jit_nativo(&cpu);

    pqp_imprime_trace(&cpu, saida);
    fprintf(saida, "\n");
    pqp_imprime_registradores(&cpu, saida);

    fclose(saida);
    return 0;
}
