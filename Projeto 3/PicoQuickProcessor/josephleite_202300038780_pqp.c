#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>

#define TAM_MEMORIA 256
#define QTD_REGISTRADORES 16
#define TAM_CODIGO_GERADO 65536
#define MAX_CORRECOES 4096
#define POS_INVALIDA 0xFFFFFFFFu
typedef struct {
    uint8_t memoria[TAM_MEMORIA + 4];
    uint8_t memoria_inicial[TAM_MEMORIA];

    uint32_t regs[QTD_REGISTRADORES];
    uint32_t pc;

    uint8_t igual;
    uint8_t maior;
    uint8_t menor;

    uint32_t contagem[TAM_MEMORIA];
    uint32_t pc_final;

    uint8_t codigo_modificado;
    uint64_t passos_totais;
} PQP;

typedef struct {
    uint32_t pos;
    uint32_t pc_alvo;
    uint8_t ir_para_saida;
} Correcao;

typedef struct {
    uint8_t* codigo_gerado;
    uint32_t pos;

    uint32_t rotulo[TAM_MEMORIA];

    Correcao correcoes[MAX_CORRECOES];
    uint32_t qtd_correcoes;

    uint32_t pos_saida;
} JIT;

void pqp_inicializar(PQP* cpu) {
    memset(cpu, 0, sizeof(PQP));
}

void pqp_carregar_arquivo_aberto(PQP* cpu, FILE* arquivo) {
    uint32_t valor_lido = 0;
    uint32_t i = 0;

    memset(cpu->memoria, 0, sizeof(cpu->memoria));
    memset(cpu->memoria_inicial, 0, sizeof(cpu->memoria_inicial));

    while(i < TAM_MEMORIA && fscanf(arquivo, "%X", &valor_lido) == 1) {
        cpu->memoria[i++] = valor_lido & 0xFF;
    }

    memcpy(cpu->memoria_inicial, cpu->memoria, TAM_MEMORIA);
}

static void __attribute__((unused)) pqp_copiar_inicio(PQP* dest, const PQP* orig) {
    pqp_inicializar(dest);
    memcpy(dest->memoria, orig->memoria_inicial, TAM_MEMORIA);
    memcpy(dest->memoria_inicial, orig->memoria_inicial, TAM_MEMORIA);
}

static uint32_t ler32(PQP* cpu, uint32_t endereco) {
    uint8_t a = (uint8_t)endereco;
    uint32_t valor;

    memcpy(&valor, &cpu->memoria[a], sizeof(uint32_t));
    return valor;
}

static void escrever32(PQP* cpu, uint32_t endereco, uint32_t valor) {
    uint8_t a = (uint8_t)endereco;

    memcpy(&cpu->memoria[a], &valor, sizeof(uint32_t));
    cpu->codigo_modificado = 0;
}

static uint32_t calcula_salto(uint32_t pc, uint16_t imm16) {
    return (uint16_t)(pc + 4 + imm16);
}

static void __attribute__((unused)) op_mover_imediato(PQP* cpu, uint32_t rx, uint32_t valor) {
    cpu->regs[rx] = valor;
}

static void __attribute__((unused)) op_mover_reg(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->regs[rx] = cpu->regs[ry];
}

static void __attribute__((unused)) op_ler_memoria(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->regs[rx] = ler32(cpu, cpu->regs[ry]);
}

static void __attribute__((unused)) op_escrever_memoria(PQP* cpu, uint32_t rx, uint32_t ry) {
    escrever32(cpu, cpu->regs[rx], cpu->regs[ry]);
}

static void __attribute__((unused)) op_comparar(PQP* cpu, uint32_t rx, uint32_t ry) {

    int32_t a = (int32_t)cpu->regs[rx];
    int32_t b = (int32_t)cpu->regs[ry];

    cpu->igual = a == b;
    cpu->maior = a > b;
    cpu->menor = a < b;
}

static void __attribute__((unused)) op_somar(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->regs[rx] += cpu->regs[ry];
}

static void __attribute__((unused)) op_subtrair(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->regs[rx] -= cpu->regs[ry];
}

static void __attribute__((unused)) op_e(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->regs[rx] &= cpu->regs[ry];
}

static void __attribute__((unused)) op_ou(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->regs[rx] |= cpu->regs[ry];
}

static void __attribute__((unused)) op_ou_exclusivo(PQP* cpu, uint32_t rx, uint32_t ry) {
    cpu->regs[rx] ^= cpu->regs[ry];
}

static void __attribute__((unused)) op_deslocar_esq(PQP* cpu, uint32_t rx, uint32_t i5) {
    cpu->regs[rx] <<= i5;
}

static void __attribute__((unused)) op_deslocar_dir(PQP* cpu, uint32_t rx, uint32_t i5) {
    cpu->regs[rx] =
        (uint32_t)(((int32_t)cpu->regs[rx]) >> i5);
}

static void __attribute__((unused)) pqp_interpretador(PQP* cpu) {
    cpu->pc = 0;
    cpu->passos_totais = 0;

    while(cpu->pc < TAM_MEMORIA) {
        if(cpu->pc + 3 >= TAM_MEMORIA) {
            break;
        }
#if LIMITE_INSTRUCOES > 0
        if(cpu->passos_totais >= LIMITE_INSTRUCOES) {
            break;
        }
#endif

        cpu->passos_totais++;
        cpu->contagem[cpu->pc]++;

        uint8_t operacao = cpu->memoria[cpu->pc];
        uint8_t b1 = cpu->memoria[cpu->pc + 1];
        uint8_t b2 = cpu->memoria[cpu->pc + 2];
        uint8_t b3 = cpu->memoria[cpu->pc + 3];

        uint8_t rx = (b1 >> 4) & 0x0F;
        uint8_t ry = b1 & 0x0F;

        uint16_t imm16 = (uint16_t)b2 | ((uint16_t)b3 << 8);
        int16_t i16 = (int16_t)imm16;
        uint8_t i5 = b3 & 0x1F;

        switch(operacao) {
            case 0x00:
                cpu->regs[rx] = (uint32_t)(int32_t)i16;
                cpu->pc += 4;
                break;

            case 0x01:
                cpu->regs[rx] = cpu->regs[ry];
                cpu->pc += 4;
                break;

            case 0x02:
                cpu->regs[rx] = ler32(cpu, cpu->regs[ry]);
                cpu->pc += 4;
                break;

            case 0x03:
                escrever32(cpu, cpu->regs[rx], cpu->regs[ry]);
                cpu->pc += 4;
                break;

            case 0x04: {

                int32_t a = (int32_t)cpu->regs[rx];
                int32_t b = (int32_t)cpu->regs[ry];

                cpu->igual = a == b;
                cpu->maior = a > b;
                cpu->menor = a < b;
                cpu->pc += 4;
                break;
            }

            case 0x05:
                cpu->pc = calcula_salto(cpu->pc, imm16);
                break;

            case 0x06:
                if(cpu->maior) {
                    cpu->pc = calcula_salto(cpu->pc, imm16);
                } else {
                    cpu->pc += 4;
                }
                break;

            case 0x07:
                if(cpu->menor) {
                    cpu->pc = calcula_salto(cpu->pc, imm16);
                } else {
                    cpu->pc += 4;
                }
                break;

            case 0x08:
                if(cpu->igual) {
                    cpu->pc = calcula_salto(cpu->pc, imm16);
                } else {
                    cpu->pc += 4;
                }
                break;

            case 0x09:
                cpu->regs[rx] += cpu->regs[ry];
                cpu->pc += 4;
                break;

            case 0x0A:
                cpu->regs[rx] -= cpu->regs[ry];
                cpu->pc += 4;
                break;

            case 0x0B:
                cpu->regs[rx] &= cpu->regs[ry];
                cpu->pc += 4;
                break;

            case 0x0C:
                cpu->regs[rx] |= cpu->regs[ry];
                cpu->pc += 4;
                break;

            case 0x0D:
                cpu->regs[rx] ^= cpu->regs[ry];
                cpu->pc += 4;
                break;

            case 0x0E:
                cpu->regs[rx] <<= i5;
                cpu->pc += 4;
                break;

            case 0x0F:
                cpu->regs[rx] =
                    (uint32_t)(((int32_t)cpu->regs[rx]) >> i5);
                cpu->pc += 4;
                break;

            default:
                exit(1);
        }
    }

    cpu->pc_final = cpu->pc;
}

static void emitir8(JIT* jit, uint8_t valor) {
    if(jit->pos >= TAM_CODIGO_GERADO) {
        exit(1);
    }

    jit->codigo_gerado[jit->pos++] = valor;
}

static void emitir32(JIT* jit, uint32_t valor) {
    emitir8(jit, valor & 0xFF);
    emitir8(jit, (valor >> 8) & 0xFF);
    emitir8(jit, (valor >> 16) & 0xFF);
    emitir8(jit, (valor >> 24) & 0xFF);
}

static void corrigir_rel32(JIT* jit, uint32_t pos, uint32_t dest) {
    int32_t rel = (int32_t)dest - (int32_t)(pos + 4);

    jit->codigo_gerado[pos + 0] = rel & 0xFF;
    jit->codigo_gerado[pos + 1] = (rel >> 8) & 0xFF;
    jit->codigo_gerado[pos + 2] = (rel >> 16) & 0xFF;
    jit->codigo_gerado[pos + 3] = (rel >> 24) & 0xFF;
}

static void adicionar_correcao(JIT* jit, uint32_t pos, uint32_t pc_alvo, uint8_t epilogue) {
    if(jit->qtd_correcoes >= MAX_CORRECOES) {
        exit(1);
    }

    jit->correcoes[jit->qtd_correcoes].pos = pos;
    jit->correcoes[jit->qtd_correcoes].pc_alvo = pc_alvo;
    jit->correcoes[jit->qtd_correcoes].ir_para_saida = epilogue;
    jit->qtd_correcoes++;
}

static void emitir_salto_com_correcao(JIT* jit, uint32_t pc_alvo) {
    emitir8(jit, 0xE9);
    uint32_t pos = jit->pos;
    emitir32(jit, 0);
    adicionar_correcao(jit, pos, pc_alvo, 0);
}

static void emitir_salto_saida(JIT* jit) {
    emitir8(jit, 0xE9);
    uint32_t pos = jit->pos;
    emitir32(jit, 0);
    adicionar_correcao(jit, pos, 0, 1);
}

static void emitir_jne_com_correcao(JIT* jit, uint32_t pc_alvo) {
    emitir8(jit, 0x0F);
    emitir8(jit, 0x85);
    uint32_t pos = jit->pos;
    emitir32(jit, 0);
    adicionar_correcao(jit, pos, pc_alvo, 0);
}

static void emitir_je_com_correcao(JIT* jit, uint32_t pc_alvo) {
    emitir8(jit, 0x0F);
    emitir8(jit, 0x84);
    uint32_t pos = jit->pos;
    emitir32(jit, 0);
    adicionar_correcao(jit, pos, pc_alvo, 0);
}

static void emitir_je_pendente(JIT* jit, uint32_t* pos_pendente) {
    emitir8(jit, 0x0F);
    emitir8(jit, 0x84);
    *pos_pendente = jit->pos;
    emitir32(jit, 0);
}

static void emitir_mov_mem32_imm32(JIT* jit, uint32_t offset, uint32_t valor) {
    emitir8(jit, 0xC7);
    emitir8(jit, 0x83);
    emitir32(jit, offset);
    emitir32(jit, valor);
}

static void emitir_inc_mem32(JIT* jit, uint32_t offset) {
    emitir8(jit, 0xFF);
    emitir8(jit, 0x83);
    emitir32(jit, offset);
}

static void emitir_cmp_mem8_imm8(JIT* jit, uint32_t offset, uint8_t valor) {
    emitir8(jit, 0x80);
    emitir8(jit, 0xBB);
    emitir32(jit, offset);
    emitir8(jit, valor);
}

static void emitir_cmp_mem32_imm32(JIT* jit, uint32_t offset, uint32_t valor) {
    emitir8(jit, 0x81);
    emitir8(jit, 0xBB);
    emitir32(jit, offset);
    emitir32(jit, valor);
}

static uint32_t desloc_reg(uint32_t r) {
    return (uint32_t)offsetof(PQP, regs) + r * sizeof(uint32_t);
}

static uint32_t desloc_mem(void) {
    return (uint32_t)offsetof(PQP, memoria);
}

static void emitir_mov_eax_base(JIT* jit, uint32_t offset) {
    emitir8(jit, 0x8B);
    emitir8(jit, 0x83);
    emitir32(jit, offset);
}

static void __attribute__((unused)) emitir_mov_edx_base(JIT* jit, uint32_t offset) {
    emitir8(jit, 0x8B);
    emitir8(jit, 0x93);
    emitir32(jit, offset);
}

static void emitir_mov_base_eax(JIT* jit, uint32_t offset) {
    emitir8(jit, 0x89);
    emitir8(jit, 0x83);
    emitir32(jit, offset);
}

static void __attribute__((unused)) emitir_mov_base_edx(JIT* jit, uint32_t offset) {
    emitir8(jit, 0x89);
    emitir8(jit, 0x93);
    emitir32(jit, offset);
}

static void emitir_mov_reg_imm32(JIT* jit, uint32_t r, uint32_t valor) {
    emitir_mov_mem32_imm32(jit, desloc_reg(r), valor);
}

static void emitir_add_reg_eax(JIT* jit, uint32_t rx) {
    emitir8(jit, 0x01);
    emitir8(jit, 0x83);
    emitir32(jit, desloc_reg(rx));
}

static void emitir_sub_reg_eax(JIT* jit, uint32_t rx) {
    emitir8(jit, 0x29);
    emitir8(jit, 0x83);
    emitir32(jit, desloc_reg(rx));
}

static void emitir_and_reg_eax(JIT* jit, uint32_t rx) {
    emitir8(jit, 0x21);
    emitir8(jit, 0x83);
    emitir32(jit, desloc_reg(rx));
}

static void emitir_or_reg_eax(JIT* jit, uint32_t rx) {
    emitir8(jit, 0x09);
    emitir8(jit, 0x83);
    emitir32(jit, desloc_reg(rx));
}

static void emitir_xor_reg_eax(JIT* jit, uint32_t rx) {
    emitir8(jit, 0x31);
    emitir8(jit, 0x83);
    emitir32(jit, desloc_reg(rx));
}

static void emitir_shl_reg_imm8(JIT* jit, uint32_t rx, uint8_t imm) {
    emitir8(jit, 0xC1);
    emitir8(jit, 0xA3);
    emitir32(jit, desloc_reg(rx));
    emitir8(jit, imm);
}

static void emitir_sar_reg_imm8(JIT* jit, uint32_t rx, uint8_t imm) {
    emitir8(jit, 0xC1);
    emitir8(jit, 0xBB);
    emitir32(jit, desloc_reg(rx));
    emitir8(jit, imm);
}

static void emitir_movzx_eax_al(JIT* jit) {
    emitir8(jit, 0x0F);
    emitir8(jit, 0xB6);
    emitir8(jit, 0xC0);
}

static void emitir_endereco_memoria(JIT* jit) {
    emitir8(jit, 0x48);
    emitir8(jit, 0x8D);
    emitir8(jit, 0x93);
    emitir32(jit, desloc_mem());
}

static void emitir_ler_memoria_indexada(JIT* jit) {
    emitir8(jit, 0x8B);
    emitir8(jit, 0x04);
    emitir8(jit, 0x02);
}

static void emitir_escrever_memoria_indexada(JIT* jit) {
    emitir8(jit, 0x89);
    emitir8(jit, 0x0C);
    emitir8(jit, 0x02);
}

static void emitir_cmp_reg_reg(JIT* jit, uint32_t rx, uint32_t ry) {
    emitir_mov_eax_base(jit, desloc_reg(rx));
    emitir8(jit, 0x3B);
    emitir8(jit, 0x83);
    emitir32(jit, desloc_reg(ry));
}

static void emitir_setcc_mem8(JIT* jit, uint8_t cc, uint32_t offset) {
    emitir8(jit, 0x0F);
    emitir8(jit, cc);
    emitir8(jit, 0x83);
    emitir32(jit, offset);
}

static void __attribute__((unused)) emitir_mov_mem8_imm8(JIT* jit, uint32_t offset, uint8_t valor) {
    emitir8(jit, 0xC6);
    emitir8(jit, 0x83);
    emitir32(jit, offset);
    emitir8(jit, valor);
}

static void emitir_mov_reg_mem(JIT* jit, uint32_t rx, uint32_t ry) {
    emitir_mov_eax_base(jit, desloc_reg(ry));
    emitir_movzx_eax_al(jit);
    emitir_endereco_memoria(jit);
    emitir_ler_memoria_indexada(jit);
    emitir_mov_base_eax(jit, desloc_reg(rx));
}

static void emitir_mov_mem_reg(JIT* jit, uint32_t rx, uint32_t ry) {
    emitir_mov_eax_base(jit, desloc_reg(rx));
    emitir_movzx_eax_al(jit);
    emitir_endereco_memoria(jit);

    emitir8(jit, 0x8B);
    emitir8(jit, 0x8B);
    emitir32(jit, desloc_reg(ry));

    emitir_escrever_memoria_indexada(jit);
}

#if LIMITE_INSTRUCOES > 0
static void emitir_cmp_mem64_imm32(JIT* jit, uint32_t offset, uint32_t valor) {

    emitir8(jit, 0x48);
    emitir8(jit, 0x81);
    emitir8(jit, 0xBB);
    emitir32(jit, offset);
    emitir32(jit, valor);
}

#endif

#if LIMITE_INSTRUCOES > 0
static void emitir_jae_saida(JIT* jit) {
    emitir8(jit, 0x0F);
    emitir8(jit, 0x83);

    uint32_t pos = jit->pos;
    emitir32(jit, 0);

    adicionar_correcao(jit, pos, 0, 1);
}
#endif

static int pc_valido(uint32_t pc) {
    return pc < TAM_MEMORIA && pc + 3 < TAM_MEMORIA && (pc % 4 == 0);
}

static void emitir_ir_para_pc(JIT* jit, uint32_t pc) {
    if(pc_valido(pc)) {
        emitir_salto_com_correcao(jit, pc);
    } else {
        emitir_mov_mem32_imm32(jit, (uint32_t)offsetof(PQP, pc), pc);
        emitir_salto_saida(jit);
    }
}

static void emitir_sair_se_pc_invalido(JIT* jit, uint32_t pc) {
    if(!pc_valido(pc)) {
        emitir_mov_mem32_imm32(jit, (uint32_t)offsetof(PQP, pc), pc);
        emitir_salto_saida(jit);
    }
}

static void emitir_jne_para_pc_ou_saida(JIT* jit, uint32_t pc) {
    if(pc_valido(pc)) {
        emitir_jne_com_correcao(jit, pc);
    } else {
        uint32_t pos_false_path;

        emitir_je_pendente(jit, &pos_false_path);

        emitir_mov_mem32_imm32(jit, (uint32_t)offsetof(PQP, pc), pc);
        emitir_salto_saida(jit);

        corrigir_rel32(jit, pos_false_path, jit->pos);
    }
}

static void __attribute__((unused)) emitir_verificar_modificacao(JIT* jit) {
    emitir_cmp_mem8_imm8(jit, (uint32_t)offsetof(PQP, codigo_modificado), 0);
    emitir_jne_com_correcao(jit, 0);
    jit->correcoes[jit->qtd_correcoes - 1].ir_para_saida = 1;
}

static void emitir_verificar_limite(JIT* jit) {
#if LIMITE_INSTRUCOES > 0
    emitir_cmp_mem64_imm32(
        jit,
        (uint32_t)offsetof(PQP, passos_totais),
        (uint32_t)LIMITE_INSTRUCOES
    );

    emitir_jae_saida(jit);
#else
    (void)jit;
#endif
}

static void aplicar_correcoes(JIT* jit) {
    for(uint32_t i = 0; i < jit->qtd_correcoes; i++) {
        uint32_t dest;

        if(jit->correcoes[i].ir_para_saida) {
            dest = jit->pos_saida;
        } else {
            uint32_t pc = jit->correcoes[i].pc_alvo;

            if(pc >= TAM_MEMORIA || jit->rotulo[pc] == POS_INVALIDA) {
                dest = jit->pos_saida;
            } else {
                dest = jit->rotulo[pc];
            }
        }

        corrigir_rel32(jit, jit->correcoes[i].pos, dest);
    }
}


static void marcar_inicios_de_bloco(PQP* cpu, uint8_t inicio_bloco[TAM_MEMORIA]) {
    memset(inicio_bloco, 0, TAM_MEMORIA);

    inicio_bloco[0] = 1;

    for(uint32_t pc = 0; pc + 3 < TAM_MEMORIA; pc += 4) {
        uint8_t operacao = cpu->memoria[pc];
        uint16_t imm16 = (uint16_t)cpu->memoria[pc + 2] |
                         ((uint16_t)cpu->memoria[pc + 3] << 8);

        if(operacao >= 0x05 && operacao <= 0x08) {
            uint32_t destino = calcula_salto(pc, imm16);

            if(pc_valido(destino)) {
                inicio_bloco[destino] = 1;
            }

            if(operacao != 0x05 && pc_valido(pc + 4)) {
                inicio_bloco[pc + 4] = 1;
            }
        }
    }
}

static uint32_t jit_compilar(PQP* cpu, uint8_t* buffer) {
    JIT jit;
    uint8_t inicio_bloco[TAM_MEMORIA];

    memset(&jit, 0, sizeof(JIT));
    jit.codigo_gerado = buffer;

    marcar_inicios_de_bloco(cpu, inicio_bloco);

    for(int i = 0; i < TAM_MEMORIA; i++) {
        jit.rotulo[i] = POS_INVALIDA;
    }

    uint32_t off_pc = (uint32_t)offsetof(PQP, pc);
    uint32_t off_exec = (uint32_t)offsetof(PQP, contagem);
    uint32_t off_igual = (uint32_t)offsetof(PQP, igual);
    uint32_t off_maior = (uint32_t)offsetof(PQP, maior);
    uint32_t off_menor = (uint32_t)offsetof(PQP, menor);

    emitir8(&jit, 0x53);
    emitir8(&jit, 0x48);
    emitir8(&jit, 0x89);
    emitir8(&jit, 0xFB);

    for(uint32_t pc = 0; pc + 3 < TAM_MEMORIA; pc += 4) {
        emitir_cmp_mem32_imm32(&jit, off_pc, pc);
        emitir_je_com_correcao(&jit, pc);
    }

    emitir_salto_saida(&jit);

    for(uint32_t pc = 0; pc + 3 < TAM_MEMORIA; pc += 4) {
        jit.rotulo[pc] = jit.pos;

        uint8_t operacao = cpu->memoria[pc];
        uint8_t b1 = cpu->memoria[pc + 1];
        uint8_t b2 = cpu->memoria[pc + 2];
        uint8_t b3 = cpu->memoria[pc + 3];

        uint8_t rx = (b1 >> 4) & 0x0F;
        uint8_t ry = b1 & 0x0F;

        uint16_t imm16 = (uint16_t)b2 | ((uint16_t)b3 << 8);
        int16_t i16 = (int16_t)imm16;
        uint8_t i5 = b3 & 0x1F;

        uint32_t proximo_pc = pc + 4;
        uint32_t destino = calcula_salto(pc, imm16);

        emitir_verificar_limite(&jit);

        if(inicio_bloco[pc]) {
            emitir_inc_mem32(&jit, off_exec + pc * sizeof(uint32_t));
        }

        switch(operacao) {
            case 0x00:
                emitir_mov_reg_imm32(&jit, rx, (uint32_t)(int32_t)i16);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x01:
                emitir_mov_eax_base(&jit, desloc_reg(ry));
                emitir_mov_base_eax(&jit, desloc_reg(rx));
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x02:
                emitir_mov_reg_mem(&jit, rx, ry);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x03:
                emitir_mov_mem_reg(&jit, rx, ry);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x04:
                emitir_cmp_reg_reg(&jit, rx, ry);
                emitir_setcc_mem8(&jit, 0x94, off_igual);
                emitir_setcc_mem8(&jit, 0x9F, off_maior);
                emitir_setcc_mem8(&jit, 0x9C, off_menor);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x05:
                emitir_ir_para_pc(&jit, destino);
                break;

            case 0x06:
                emitir_cmp_mem8_imm8(&jit, off_maior, 0);
                emitir_jne_para_pc_ou_saida(&jit, destino);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x07:
                emitir_cmp_mem8_imm8(&jit, off_menor, 0);
                emitir_jne_para_pc_ou_saida(&jit, destino);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x08:
                emitir_cmp_mem8_imm8(&jit, off_igual, 0);
                emitir_jne_para_pc_ou_saida(&jit, destino);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x09:
                emitir_mov_eax_base(&jit, desloc_reg(ry));
                emitir_add_reg_eax(&jit, rx);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x0A:
                emitir_mov_eax_base(&jit, desloc_reg(ry));
                emitir_sub_reg_eax(&jit, rx);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x0B:
                emitir_mov_eax_base(&jit, desloc_reg(ry));
                emitir_and_reg_eax(&jit, rx);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x0C:
                emitir_mov_eax_base(&jit, desloc_reg(ry));
                emitir_or_reg_eax(&jit, rx);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x0D:
                emitir_mov_eax_base(&jit, desloc_reg(ry));
                emitir_xor_reg_eax(&jit, rx);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x0E:
                emitir_shl_reg_imm8(&jit, rx, i5);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            case 0x0F:
                emitir_sar_reg_imm8(&jit, rx, i5);
                emitir_sair_se_pc_invalido(&jit, proximo_pc);
                break;

            default:
                emitir_salto_saida(&jit);
                break;
        }
    }

    jit.pos_saida = jit.pos;

    emitir8(&jit, 0x5B);
    emitir8(&jit, 0xC3);

    aplicar_correcoes(&jit);

    return jit.pos;
}

static void pqp_expandir_contagem(PQP* cpu);

void pqp_jit_nativo(PQP* cpu) {
    cpu->pc = 0;
    cpu->passos_totais = 0;
    cpu->codigo_modificado = 0;

    void* memory = mmap(
        NULL,
        TAM_CODIGO_GERADO,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,
        0
    );

    if(memory == MAP_FAILED) {
        exit(1);
    }

    jit_compilar(cpu, (uint8_t*)memory);

    if(mprotect(memory, TAM_CODIGO_GERADO, PROT_READ | PROT_EXEC) != 0) {
        munmap(memory, TAM_CODIGO_GERADO);
        exit(1);
    }

    void (*codigo_gerado)(PQP*) = (void (*)(PQP*))memory;
    codigo_gerado(cpu);

    munmap(memory, TAM_CODIGO_GERADO);

    pqp_expandir_contagem(cpu);
    cpu->pc_final = cpu->pc;
}


static void pqp_expandir_contagem(PQP* cpu) {
    uint8_t inicio_bloco[TAM_MEMORIA];

    marcar_inicios_de_bloco(cpu, inicio_bloco);

    for(uint32_t inicio = 0; inicio + 3 < TAM_MEMORIA; inicio += 4) {
        if(!inicio_bloco[inicio] || cpu->contagem[inicio] == 0) {
            continue;
        }

        uint32_t pc = inicio;

        while(pc + 3 < TAM_MEMORIA) {
            uint8_t operacao = cpu->memoria_inicial[pc];

            if(operacao >= 0x05 && operacao <= 0x08) {
                break;
            }

            uint32_t prox = pc + 4;

            if(prox + 3 >= TAM_MEMORIA || inicio_bloco[prox]) {
                break;
            }

            cpu->contagem[prox] = cpu->contagem[inicio];
            pc = prox;
        }
    }
}

void pqp_imprimir_regs(PQP* cpu, FILE* out) {
    for(int i = 0; i < QTD_REGISTRADORES; i++) {
        fprintf(out, "r%d=%08x", i, cpu->regs[i]);

        if(i < QTD_REGISTRADORES - 1) {
            fprintf(out, ",");
        }
    }

    fprintf(out, "\n");
}

static void pqp_imprimir_instrucao(PQP* cpu, uint32_t pc, FILE* out) {
    uint8_t operacao = cpu->memoria_inicial[pc];
    uint8_t b1 = cpu->memoria_inicial[pc + 1];
    uint8_t b2 = cpu->memoria_inicial[pc + 2];
    uint8_t b3 = cpu->memoria_inicial[pc + 3];

    uint8_t rx = (b1 >> 4) & 0x0F;
    uint8_t ry = b1 & 0x0F;

    uint16_t imm16 = (uint16_t)b2 | ((uint16_t)b3 << 8);
    int16_t i16 = (int16_t)imm16;
    uint8_t i5 = b3 & 0x1F;

    uint32_t destino = calcula_salto(pc, imm16);

    fprintf(out, "%13u:%04x:",
            cpu->contagem[pc],
            pc);

    switch(operacao) {
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
            fprintf(out, "jmp_%04x", destino);
            break;

        case 0x06:
            fprintf(out, "jg_%04x", destino);
            break;

        case 0x07:
            fprintf(out, "jl_%04x", destino);
            break;

        case 0x08:
            fprintf(out, "je_%04x", destino);
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
            fprintf(out, "db_%02x_%02x_%02x_%02x", operacao, b1, b2, b3);
            break;
    }

    fprintf(out, "\n");
}

void pqp_imprimir_rastro(PQP* cpu, FILE* out) {
    for(uint32_t pc = 0; pc + 3 < TAM_MEMORIA; pc += 4) {
        if(cpu->contagem[pc] > 0) {
            pqp_imprimir_instrucao(cpu, pc, out);
        }
    }

    fprintf(out, "%13c:%04x:exit\n", '-', cpu->pc_final);
}

int main(int argc, char* argv[]) {
    PQP cpu;
    FILE* entrada;
    FILE* saida;

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

    pqp_inicializar(&cpu);
    pqp_carregar_arquivo_aberto(&cpu, entrada);
    fclose(entrada);

    pqp_jit_nativo(&cpu);

    pqp_imprimir_rastro(&cpu, saida);
    fprintf(saida, "\n");
    pqp_imprimir_regs(&cpu, saida);

    fclose(saida);
    return 0;
}
