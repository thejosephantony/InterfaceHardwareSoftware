/* PicoQuickProcessor.c - versão sem aceleradores de laço */

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
#include <signal.h>
#include <setjmp.h>
#include <sys/time.h>

#define MEM_SIZE 256
#define REG_COUNT 16
#define CODE_SIZE 65536
#define MAX_PATCHES 4096
#define INVALID_POS 0xFFFFFFFFu
#define LIMITE_INSTRUCOES 0ULL
#define WATCHDOG_TEMPO_US 0

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
    memcpy(&cpu->memoria[a], &value, sizeof(uint32_t));
    cpu->jit_dirty = 0;
}

static uint32_t calcula_salto(uint32_t pc, uint16_t imm16) {
    return (uint16_t)(pc + 4 + imm16);
}

#if WATCHDOG_TEMPO_US > 0
static sigjmp_buf watchdog_env;
static volatile sig_atomic_t watchdog_ativo = 0;

static void watchdog_handler(int sig) {
    (void)sig;
    if(watchdog_ativo) {
        siglongjmp(watchdog_env, 1);
    }
}

static void watchdog_iniciar(void) {
    struct itimerval timer;
    signal(SIGALRM, watchdog_handler);
    memset(&timer, 0, sizeof(timer));
    timer.it_value.tv_sec = WATCHDOG_TEMPO_US / 1000000;
    timer.it_value.tv_usec = WATCHDOG_TEMPO_US % 1000000;
    watchdog_ativo = 1;
    setitimer(ITIMER_REAL, &timer, NULL);
}

static void watchdog_parar(void) {
    struct itimerval timer;
    memset(&timer, 0, sizeof(timer));
    setitimer(ITIMER_REAL, &timer, NULL);
    watchdog_ativo = 0;
}
#endif

/* Funções auxiliares para o JIT (emissão de código nativo) */

static void emit8(JIT* jit, uint8_t value) {
    if(jit->pos >= CODE_SIZE) exit(1);
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
    if(jit->patch_count >= MAX_PATCHES) exit(1);
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

static uint32_t off_reg(uint32_t r) {
    return (uint32_t)offsetof(PQP, registrador) + r * sizeof(uint32_t);
}

static uint32_t off_mem(void) {
    return (uint32_t)offsetof(PQP, memoria);
}

static void emit_mov_eax_membase(JIT* jit, uint32_t offset) {
    emit8(jit, 0x8B);
    emit8(jit, 0x83);
    emit32(jit, offset);
}

static void emit_mov_membase_eax(JIT* jit, uint32_t offset) {
    emit8(jit, 0x89);
    emit8(jit, 0x83);
    emit32(jit, offset);
}

static void emit_mov_reg_imm32(JIT* jit, uint32_t r, uint32_t value) {
    emit_mov_mem32_imm32(jit, off_reg(r), value);
}

static void emit_add_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x01);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

static void emit_sub_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x29);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

static void emit_and_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x21);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

static void emit_or_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x09);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

static void emit_xor_reg_eax(JIT* jit, uint32_t rx) {
    emit8(jit, 0x31);
    emit8(jit, 0x83);
    emit32(jit, off_reg(rx));
}

static void emit_shl_reg_imm8(JIT* jit, uint32_t rx, uint8_t imm) {
    emit8(jit, 0xC1);
    emit8(jit, 0xA3);
    emit32(jit, off_reg(rx));
    emit8(jit, imm);
}

static void emit_sar_reg_imm8(JIT* jit, uint32_t rx, uint8_t imm) {
    emit8(jit, 0xC1);
    emit8(jit, 0xBB);
    emit32(jit, off_reg(rx));
    emit8(jit, imm);
}

static void emit_movzx_eax_al(JIT* jit) {
    emit8(jit, 0x0F);
    emit8(jit, 0xB6);
    emit8(jit, 0xC0);
}

static void emit_lea_rdx_memoria(JIT* jit) {
    emit8(jit, 0x48);
    emit8(jit, 0x8D);
    emit8(jit, 0x93);
    emit32(jit, off_mem());
}

static void emit_mov_eax_mem_rdx_rax(JIT* jit) {
    emit8(jit, 0x8B);
    emit8(jit, 0x04);
    emit8(jit, 0x02);
}

static void emit_mov_mem_rdx_rax_ecx(JIT* jit) {
    emit8(jit, 0x89);
    emit8(jit, 0x0C);
    emit8(jit, 0x02);
}

static void emit_cmp_reg_reg_inline(JIT* jit, uint32_t rx, uint32_t ry) {
    emit_mov_eax_membase(jit, off_reg(rx));
    emit8(jit, 0x3B);
    emit8(jit, 0x83);
    emit32(jit, off_reg(ry));
}

static void emit_setcc_mem8(JIT* jit, uint8_t cc, uint32_t offset) {
    emit8(jit, 0x0F);
    emit8(jit, cc);
    emit8(jit, 0x83);
    emit32(jit, offset);
}

static void emit_mov_rm_inline(JIT* jit, uint32_t rx, uint32_t ry) {
    emit_mov_eax_membase(jit, off_reg(ry));
    emit_movzx_eax_al(jit);
    emit_lea_rdx_memoria(jit);
    emit_mov_eax_mem_rdx_rax(jit);
    emit_mov_membase_eax(jit, off_reg(rx));
}

static void emit_mov_mr_inline(JIT* jit, uint32_t rx, uint32_t ry) {
    emit_mov_eax_membase(jit, off_reg(rx));
    emit_movzx_eax_al(jit);
    emit_lea_rdx_memoria(jit);
    emit8(jit, 0x8B);
    emit8(jit, 0x8B);
    emit32(jit, off_reg(ry));
    emit_mov_mem_rdx_rax_ecx(jit);
}

#if LIMITE_INSTRUCOES > 0
static void emit_cmp_mem64_imm32(JIT* jit, uint32_t offset, uint32_t value) {
    emit8(jit, 0x48);
    emit8(jit, 0x81);
    emit8(jit, 0xBB);
    emit32(jit, offset);
    emit32(jit, value);
}

static void emit_jae_epilogue(JIT* jit) {
    emit8(jit, 0x0F);
    emit8(jit, 0x83);
    uint32_t pos = jit->pos;
    emit32(jit, 0);
    add_patch(jit, pos, 0, 1);
}
#endif

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
        emit_je_placeholder(jit, &pos_false_path);
        emit_mov_mem32_imm32(jit, (uint32_t)offsetof(PQP, pcounter), pc);
        emit_jmp_epilogue(jit);
        patch_rel32(jit, pos_false_path, jit->pos);
    }
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

    uint32_t off_pc     = (uint32_t)offsetof(PQP, pcounter);
    uint32_t off_exec   = (uint32_t)offsetof(PQP, execucoes);
    uint32_t off_igual  = (uint32_t)offsetof(PQP, igual);
    uint32_t off_maior  = (uint32_t)offsetof(PQP, maior);
    uint32_t off_menor  = (uint32_t)offsetof(PQP, menor);
    uint32_t off_ciclos = (uint32_t)offsetof(PQP, ciclos_totais);

    /* prólogo */
    emit8(&jit, 0x53);                  // push rbx
    emit8(&jit, 0x48); emit8(&jit, 0x89); emit8(&jit, 0xFB); // mov rbx, rdi

    /* despacho inicial */
    for(uint32_t pc = 0; pc + 3 < MEM_SIZE; pc += 4) {
        emit_cmp_mem32_imm32(&jit, off_pc, pc);
        emit_je_patch(&jit, pc);
    }
    emit_jmp_epilogue(&jit);

    /* blocos de instrução */
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

        /* removida chamada ao acelerador e verificação de jit_dirty */

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
                emit_setcc_mem8(&jit, 0x94, off_igual);
                emit_setcc_mem8(&jit, 0x9F, off_maior);
                emit_setcc_mem8(&jit, 0x9C, off_menor);
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
    emit8(&jit, 0x5B);  // pop rbx
    emit8(&jit, 0xC3);  // ret

    jit_patch(&jit);
    return jit.pos;
}

void pqp_jit_nativo(PQP* cpu) {
    cpu->pcounter = 0;
    cpu->ciclos_totais = 0;

#if WATCHDOG_TEMPO_US > 0
    if(sigsetjmp(watchdog_env, 1) != 0) {
        watchdog_parar();
        cpu->pc_saida = cpu->pcounter;
        return;
    }
    watchdog_iniciar();
#endif

    /* Compila uma vez – sem aceleradores, jit_dirty nunca será 1 */
    cpu->jit_dirty = 0;

    void* memory = mmap(NULL, CODE_SIZE,
                        PROT_READ | PROT_WRITE,
                        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if(memory == MAP_FAILED) exit(1);

    jit_compilar(cpu, (uint8_t*)memory);

    if(mprotect(memory, CODE_SIZE, PROT_READ | PROT_EXEC) != 0) {
        munmap(memory, CODE_SIZE);
        exit(1);
    }

    void (*codigo_gerado)(PQP*) = (void (*)(PQP*))memory;
    codigo_gerado(cpu);
    munmap(memory, CODE_SIZE);

#if WATCHDOG_TEMPO_US > 0
    watchdog_parar();
#endif

    cpu->pc_saida = cpu->pcounter;
}

/* Funções de impressão (trace e registradores) – mantidas como no original */

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
            (unsigned long long)cpu->execucoes[pc], pc);

    switch(opcode) {
        case 0x00: fprintf(out, "mov_r%u,%08x", rx, (uint32_t)(int32_t)i16); break;
        case 0x01: fprintf(out, "mov_r%u,r%u", rx, ry); break;
        case 0x02: fprintf(out, "mov_r%u,[r%u]", rx, ry); break;
        case 0x03: fprintf(out, "mov_[r%u],r%u", rx, ry); break;
        case 0x04: fprintf(out, "cmp_r%u<=>r%u", rx, ry); break;
        case 0x05: fprintf(out, "jmp_%04x", alvo); break;
        case 0x06: fprintf(out, "jg_%04x", alvo); break;
        case 0x07: fprintf(out, "jl_%04x", alvo); break;
        case 0x08: fprintf(out, "je_%04x", alvo); break;
        case 0x09: fprintf(out, "add_r%u+=r%u", rx, ry); break;
        case 0x0A: fprintf(out, "sub_r%u-=r%u", rx, ry); break;
        case 0x0B: fprintf(out, "and_r%u&=r%u", rx, ry); break;
        case 0x0C: fprintf(out, "or_r%u|=r%u", rx, ry); break;
        case 0x0D: fprintf(out, "xor_r%u^=r%u", rx, ry); break;
        case 0x0E: fprintf(out, "sal_r%u<<=%u", rx, i5); break;
        case 0x0F: fprintf(out, "sar_r%u>>=%u", rx, i5); break;
        default:   fprintf(out, "db_%02x_%02x_%02x_%02x", opcode, b1, b2, b3); break;
    }
    fprintf(out, "\n");
}

void pqp_imprime_trace(PQP* cpu, FILE* out) {
    for(uint32_t pc = 0; pc + 3 < MEM_SIZE; pc += 4) {
        if(cpu->execucoes[pc] > 0)
            pqp_imprime_instrucao(cpu, pc, out);
    }
    fprintf(out, "%13c:%04x:exit\n", '-', cpu->pc_saida);
}

void pqp_imprime_registradores(PQP* cpu, FILE* out) {
    for(int i = 0; i < REG_COUNT; i++) {
        fprintf(out, "r%d=%08x", i, cpu->registrador[i]);
        if(i < REG_COUNT - 1) fprintf(out, ",");
    }
    fprintf(out, "\n");
}

int main(int argc, char* argv[]) {
    PQP cpu;
    FILE* entrada;
    FILE* saida;

    if(argc != 3) return 1;

    entrada = fopen(argv[1], "r");
    if(entrada == NULL) return 1;

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