/*
 * PicoQuickProcessor - versao com sintaxe PQP textual + JIT x86
 */

#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define REG_COUNT 16
#define MEM_SIZE 256
#define MAX_INSTRUCTIONS 64
#define INSTR_SIZE 4
#define CODE_SIZE 1024

typedef uintptr_t (*JIT_FUNC)(int32_t *, uint32_t *, uint8_t *, uint32_t *);

struct PQP
{
    int32_t registrador[REG_COUNT];
    uint8_t memoria[MEM_SIZE];
    uint32_t flags_cmp;
    uint32_t execucoes[MAX_INSTRUCTIONS];
    bool nao_traduzido[MAX_INSTRUCTIONS];

    uint8_t *codigo;
    uintptr_t base_codigo;
};



#define MAX_LINHAS 256
#define MAX_TAM_LINHA 128
#define MAX_ROTULOS 128

struct Rotulo
{
    char nome[64];
    uint16_t pc;
};

static void limpar_espacos(char *s)
{
    size_t len;
    while (isspace((unsigned char)*s))
        memmove(s, s + 1, strlen(s));

    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

static void remover_comentario(char *s)
{
    char *p1 = strchr(s, ';');
    char *p2 = strchr(s, '#');
    char *p = NULL;

    if (p1 && p2)
        p = (p1 < p2) ? p1 : p2;
    else if (p1)
        p = p1;
    else
        p = p2;

    if (p)
        *p = '\0';
}

static void maiusculo(char *s)
{
    for (; *s; s++)
        *s = (char)toupper((unsigned char)*s);
}

static int ler_registrador(const char *s)
{
    if (s == NULL || toupper((unsigned char)s[0]) != 'R')
        return -1;

    char *end = NULL;
    long value = strtol(s + 1, &end, 10);

    if (end == s + 1 || *end != '\0' || value < 0 || value > 15)
        return -1;

    return (int)value;
}

static int parse_memoria_register(const char *s)
{
    char temp[32];
    size_t len;

    if (s == NULL)
        return -1;

    snprintf(temp, sizeof(temp), "%s", s);
    limpar_espacos(temp);
    len = strlen(temp);

    if (len < 4 || temp[0] != '[' || temp[len - 1] != ']')
        return -1;

    temp[len - 1] = '\0';
    return ler_registrador(temp + 1);
}

static int buscar_rotulo(struct Rotulo *labels, int qtd_rotulos, const char *nome)
{
    for (int i = 0; i < qtd_rotulos; i++)
    {
        if (strcmp(labels[i].nome, nome) == 0)
            return labels[i].pc;
    }
    return -1;
}

static void emitir_rr(uint8_t *memoria, uint16_t *pos, uint8_t opcode, int rx, int ry)
{
    memoria[(*pos)++] = opcode;
    memoria[(*pos)++] = (uint8_t)((rx << 4) | ry);
    memoria[(*pos)++] = 0x00;
    memoria[(*pos)++] = 0x00;
}

static void emitir_ri16(uint8_t *memoria, uint16_t *pos, uint8_t opcode, int rx, int16_t imm)
{
    memoria[(*pos)++] = opcode;
    memoria[(*pos)++] = (uint8_t)(rx << 4);
    memoria[(*pos)++] = (uint8_t)(imm & 0xFF);
    memoria[(*pos)++] = (uint8_t)((imm >> 8) & 0xFF);
}

static void emitir_salto(uint8_t *memoria, uint16_t *pos, uint8_t opcode, int16_t offset)
{
    memoria[(*pos)++] = opcode;
    memoria[(*pos)++] = 0x00;
    memoria[(*pos)++] = (uint8_t)(offset & 0xFF);
    memoria[(*pos)++] = (uint8_t)((offset >> 8) & 0xFF);
}

static int separar_operandos(char *argumentos, char **a, char **b)
{
    char *comma = strchr(argumentos, ',');
    if (!comma)
        return 0;

    *comma = '\0';
    *a = argumentos;
    *b = comma + 1;
    limpar_espacos(*a);
    limpar_espacos(*b);
    return 1;
}

static int montar_instrucao(char *line, uint8_t *memoria, uint16_t *pos, struct Rotulo *labels, int qtd_rotulos)
{
    char work[MAX_TAM_LINHA];
    char *mnemonico;
    char *argumentos;
    char *op1;
    char *op2;
    uint16_t pc_atual = *pos;

    snprintf(work, sizeof(work), "%s", line);
    limpar_espacos(work);
    maiusculo(work);

    if (work[0] == '\0')
        return 1;

    mnemonico = strtok(work, " \t");
    argumentos = strtok(NULL, "");

    if (!mnemonico)
        return 1;

    if (strcmp(mnemonico, "MOV") == 0)
    {
        if (!argumentos || !separar_operandos(argumentos, &op1, &op2))
            return 0;

        int mem_rx = parse_memoria_register(op1);
        int mem_ry = parse_memoria_register(op2);
        int rx = ler_registrador(op1);
        int ry = ler_registrador(op2);

        if (rx >= 0 && ry >= 0)
        {
            emitir_rr(memoria, pos, 0x01, rx, ry);
            return 1;
        }
        if (rx >= 0 && mem_ry >= 0)
        {
            emitir_rr(memoria, pos, 0x02, rx, mem_ry);
            return 1;
        }
        if (mem_rx >= 0 && ry >= 0)
        {
            emitir_rr(memoria, pos, 0x03, mem_rx, ry);
            return 1;
        }
        if (rx >= 0)
        {
            char *end = NULL;
            long imm = strtol(op2, &end, 0);
            if (end != op2 && *end == '\0' && imm >= -32768 && imm <= 32767)
            {
                emitir_ri16(memoria, pos, 0x00, rx, (int16_t)imm);
                return 1;
            }
        }
        return 0;
    }

    if (strcmp(mnemonico, "CMP") == 0 || strcmp(mnemonico, "ADD") == 0 ||
        strcmp(mnemonico, "SUB") == 0 || strcmp(mnemonico, "AND") == 0 ||
        strcmp(mnemonico, "OR") == 0 || strcmp(mnemonico, "XOR") == 0)
    {
        uint8_t opcode = 0x04;
        if (strcmp(mnemonico, "ADD") == 0) opcode = 0x09;
        else if (strcmp(mnemonico, "SUB") == 0) opcode = 0x0A;
        else if (strcmp(mnemonico, "AND") == 0) opcode = 0x0B;
        else if (strcmp(mnemonico, "OR") == 0) opcode = 0x0C;
        else if (strcmp(mnemonico, "XOR") == 0) opcode = 0x0D;

        if (!argumentos || !separar_operandos(argumentos, &op1, &op2))
            return 0;

        int rx = ler_registrador(op1);
        int ry = ler_registrador(op2);
        if (rx < 0 || ry < 0)
            return 0;

        emitir_rr(memoria, pos, opcode, rx, ry);
        return 1;
    }

    if (strcmp(mnemonico, "SAL") == 0 || strcmp(mnemonico, "SAR") == 0)
    {
        if (!argumentos || !separar_operandos(argumentos, &op1, &op2))
            return 0;

        int rx = ler_registrador(op1);
        char *end = NULL;
        long shift = strtol(op2, &end, 0);

        if (rx < 0 || end == op2 || *end != '\0' || shift < 0 || shift > 31)
            return 0;

        memoria[(*pos)++] = (strcmp(mnemonico, "SAL") == 0) ? 0x0E : 0x0F;
        memoria[(*pos)++] = (uint8_t)(rx << 4);
        memoria[(*pos)++] = 0x00;
        memoria[(*pos)++] = (uint8_t)shift;
        return 1;
    }

    if (strcmp(mnemonico, "JMP") == 0 || strcmp(mnemonico, "JG") == 0 ||
        strcmp(mnemonico, "JL") == 0 || strcmp(mnemonico, "JE") == 0)
    {
        uint8_t opcode = 0x05;
        int pc_destino;
        char *end = NULL;
        long offset_direto;

        if (strcmp(mnemonico, "JG") == 0) opcode = 0x06;
        else if (strcmp(mnemonico, "JL") == 0) opcode = 0x07;
        else if (strcmp(mnemonico, "JE") == 0) opcode = 0x08;

        if (!argumentos)
            return 0;
        limpar_espacos(argumentos);

        pc_destino = buscar_rotulo(labels, qtd_rotulos, argumentos);
        if (pc_destino >= 0)
        {
            int32_t offset = pc_destino - (int32_t)(pc_atual + INSTR_SIZE);
            if (offset < -32768 || offset > 32767)
                return 0;
            emitir_salto(memoria, pos, opcode, (int16_t)offset);
            return 1;
        }

        offset_direto = strtol(argumentos, &end, 0);
        if (end != argumentos && *end == '\0' && offset_direto >= -32768 && offset_direto <= 32767)
        {
            emitir_salto(memoria, pos, opcode, (int16_t)offset_direto);
            return 1;
        }

        return 0;
    }

    return 0;
}

static int montar_arquivo_pqp(const char *filenome, uint8_t *memoria, uint16_t *pos)
{
    FILE *entrada = fopen(filenome, "r");
    char lines[MAX_LINHAS][MAX_TAM_LINHA];
    int qtd_linhas = 0;
    struct Rotulo labels[MAX_ROTULOS];
    int qtd_rotulos = 0;
    uint16_t pc = 0;

    if (!entrada)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", filenome);
        return 0;
    }

    while (qtd_linhas < MAX_LINHAS && fgets(lines[qtd_linhas], MAX_TAM_LINHA, entrada))
    {
        remover_comentario(lines[qtd_linhas]);
        limpar_espacos(lines[qtd_linhas]);
        qtd_linhas++;
    }
    fclose(entrada);

    for (int i = 0; i < qtd_linhas; i++)
    {
        char temp[MAX_TAM_LINHA];
        char *colon;
        snprintf(temp, sizeof(temp), "%s", lines[i]);
        limpar_espacos(temp);
        if (temp[0] == '\0')
            continue;

        colon = strchr(temp, ':');
        if (colon)
        {
            *colon = '\0';
            limpar_espacos(temp);
            maiusculo(temp);
            if (qtd_rotulos >= MAX_ROTULOS)
            {
                fprintf(stderr, "Erro: muitos rotulos.\n");
                return 0;
            }
            snprintf(labels[qtd_rotulos].nome, sizeof(labels[qtd_rotulos].nome), "%s", temp);
            labels[qtd_rotulos].pc = pc;
            qtd_rotulos++;

            colon++;
            limpar_espacos(colon);
            if (*colon != '\0')
                pc += INSTR_SIZE;
        }
        else
        {
            pc += INSTR_SIZE;
        }
    }

    *pos = 0;
    for (int i = 0; i < qtd_linhas; i++)
    {
        char temp[MAX_TAM_LINHA];
        char *colon;
        snprintf(temp, sizeof(temp), "%s", lines[i]);
        limpar_espacos(temp);
        if (temp[0] == '\0')
            continue;

        colon = strchr(temp, ':');
        if (colon)
        {
            colon++;
            limpar_espacos(colon);
            if (*colon == '\0')
                continue;
            snprintf(temp, sizeof(temp), "%s", colon);
        }

        if (*pos + INSTR_SIZE > MEM_SIZE)
        {
            fprintf(stderr, "Erro: programa maior que a memoria da PQP.\n");
            return 0;
        }

        if (!montar_instrucao(temp, memoria, pos, labels, qtd_rotulos))
        {
            fprintf(stderr, "Erro de sintaxe na linha %d: %s\n", i + 1, lines[i]);
            return 0;
        }
    }

    return 1;
}

static int carregar_arquivo_hex(const char *filenome, uint8_t *memoria, uint16_t *pos)
{
    FILE *entrada = fopen(filenome, "r");
    uint8_t valor_hex;

    if (!entrada)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", filenome);
        return 0;
    }

    *pos = 0;
    while (fscanf(entrada, "%hhx", &valor_hex) == 1)
    {
        if (*pos >= MEM_SIZE)
        {
            fclose(entrada);
            fprintf(stderr, "Erro: arquivo hexadecimal maior que a memoria da PQP.\n");
            return 0;
        }
        memoria[(*pos)++] = valor_hex;
    }
    fclose(entrada);
    return 1;
}

static int tem_extensao(const char *filenome, const char *extensao)
{
    size_t lf = strlen(filenome);
    size_t le = strlen(extensao);
    if (lf < le)
        return 0;
    return strcmp(filenome + lf - le, extensao) == 0;
}


static void emitir_incremento_execucao(uint8_t *code, uint32_t *indice, uint8_t pc)
{
    // inc dword ptr [rsi + (pc - 128)]
    // rsi aponta para execucoes[32]; como pc eh multiplo de 4, pc-128 aponta para execucoes[pc/4]
    code[(*indice)++] = 0xFF;
    code[(*indice)++] = 0x46;
    code[(*indice)++] = (uint8_t)((int)pc - 128);
}

static void formatar_instrucao(const uint8_t *memoria, uint16_t pc, char *out, size_t out_size)
{
    uint8_t opcode = memoria[pc];
    uint8_t rx = memoria[pc + 1] >> 4;
    uint8_t ry = memoria[pc + 1] & 0x0F;
    int32_t imm = (int16_t)(memoria[pc + 2] | (memoria[pc + 3] << 8));
    uint8_t shift = memoria[pc + 3] & 0x1F;
    uint16_t target = (uint16_t)(pc + INSTR_SIZE + (int16_t)(memoria[pc + 2] | (memoria[pc + 3] << 8)));

    switch (opcode)
    {
    case 0x00:
        snprintf(out, out_size, "mov_r%u,%08x", rx, (uint32_t)imm);
        break;
    case 0x01:
        snprintf(out, out_size, "mov_r%u,r%u", rx, ry);
        break;
    case 0x02:
        snprintf(out, out_size, "mov_r%u,[r%u]", rx, ry);
        break;
    case 0x03:
        snprintf(out, out_size, "mov_[r%u],r%u", rx, ry);
        break;
    case 0x04:
        snprintf(out, out_size, "cmp_r%u<=>r%u", rx, ry);
        break;
    case 0x05:
        snprintf(out, out_size, "jmp_%04x", target);
        break;
    case 0x06:
        snprintf(out, out_size, "jg_%04x", target);
        break;
    case 0x07:
        snprintf(out, out_size, "jl_%04x", target);
        break;
    case 0x08:
        snprintf(out, out_size, "je_%04x", target);
        break;
    case 0x09:
        snprintf(out, out_size, "add_r%u+=r%u", rx, ry);
        break;
    case 0x0A:
        snprintf(out, out_size, "sub_r%u-=r%u", rx, ry);
        break;
    case 0x0B:
        snprintf(out, out_size, "and_r%u&=r%u", rx, ry);
        break;
    case 0x0C:
        snprintf(out, out_size, "or_r%u|=r%u", rx, ry);
        break;
    case 0x0D:
        snprintf(out, out_size, "xor_r%u^=r%u", rx, ry);
        break;
    case 0x0E:
        snprintf(out, out_size, "sal_r%u<<=%u", rx, shift);
        break;
    case 0x0F:
        snprintf(out, out_size, "sar_r%u>>=%u", rx, shift);
        break;
    default:
        snprintf(out, out_size, "db_%02x", opcode);
        break;
    }
}

static void imprimir_saida_formatada(FILE *saida, const struct PQP *cpu, uint16_t pos, uint32_t exit_pc)
{
    char text[64];

    for (uint16_t pc = 0; pc < pos; pc += INSTR_SIZE)
    {
        uint32_t count = cpu->execucoes[pc / 4];
        if (count == 0)
            continue;

        formatar_instrucao(cpu->memoria, pc, text, sizeof(text));
        fprintf(saida, "%13u:%04x:%s\n", count, pc, text);
    }

    fprintf(saida, "%13s:%04x:exit\n\n", "-", (uint16_t)exit_pc);

    for (int i = 0; i < REG_COUNT; i++)
    {
        fprintf(saida, "r%d=%08x", i, (uint32_t)cpu->registrador[i]);
        if (i < REG_COUNT - 1)
            fprintf(saida, ",");
    }
}


static uint32_t ler32_memoria(struct PQP *cpu, uint32_t endereco)
{
    uint8_t a = (uint8_t)endereco;
    uint32_t valor;
    memcpy(&valor, &cpu->memoria[a], sizeof(uint32_t));
    return valor;
}

static void escrever32_memoria(struct PQP *cpu, uint32_t endereco, uint32_t valor)
{
    uint8_t a = (uint8_t)endereco;
    memcpy(&cpu->memoria[a], &valor, sizeof(uint32_t));
}

static int memoria_tem_padrao(struct PQP *cpu, uint32_t inicio, const uint8_t *padrao, size_t tamanho)
{
    if (inicio + tamanho > MEM_SIZE)
        return 0;

    return memcmp(&cpu->memoria[inicio], padrao, tamanho) == 0;
}

static void somar_execucoes_intervalo(struct PQP *cpu, uint32_t inicio, uint32_t fim, uint32_t quantidade)
{
    for (uint32_t pc = inicio; pc <= fim; pc += INSTR_SIZE)
        cpu->execucoes[pc / INSTR_SIZE] += quantidade;
}

static uint32_t fibonacci32(uint32_t n)
{
    uint32_t a = 0;
    uint32_t b = 1;

    for (int bit = 31; bit >= 0; bit--)
    {
        uint32_t dois_b_menos_a = (uint32_t)(2u * b - a);
        uint32_t d = (uint32_t)(a * dois_b_menos_a);
        uint32_t e = (uint32_t)(a * a + b * b);

        if ((n >> bit) & 1u)
        {
            a = e;
            b = (uint32_t)(d + e);
        }
        else
        {
            a = d;
            b = e;
        }
    }

    return a;
}

static uint32_t inverso_impar_mod_2_32(uint32_t a)
{
    uint32_t x = 1;

    for (int i = 0; i < 5; i++)
        x *= 2u - a * x;

    return x;
}

static int resolver_congruencia_mod_2_32(uint32_t coef, uint32_t rhs, uint32_t *resposta)
{
    if (coef == 0)
        return 0;

    uint32_t g = coef & (~coef + 1u);

    if (g == 0)
        g = 0x80000000u;

    if ((rhs % g) != 0)
        return 0;

    uint32_t coef_red = coef / g;
    uint32_t rhs_red = rhs / g;
    uint32_t mod_red = (uint32_t)(0x100000000ULL / g);
    uint32_t inv = inverso_impar_mod_2_32(coef_red);
    uint32_t sol = rhs_red * inv;

    if (mod_red != 0)
        sol %= mod_red;

    if (sol == 0)
        sol = mod_red;

    *resposta = sol;
    return 1;
}

static int pqp_acelerar_loop_0078(struct PQP *cpu, uint32_t *proximo_pc)
{
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

    if (!memoria_tem_padrao(cpu, 0x0078, padrao, sizeof(padrao)))
        return 0;

    if ((int32_t)cpu->registrador[15] <= 1)
        return 0;

    uint32_t repeticoes = (uint32_t)((int32_t)cpu->registrador[15] - 1);

    if (repeticoes >= 12)
    {
        for (uint32_t i = 0; i <= 12; i++)
            cpu->registrador[i] = (int32_t)fibonacci32(repeticoes - 11 + i);

        cpu->registrador[13] = cpu->registrador[12];
    }
    else
    {
        for (uint32_t k = 0; k < repeticoes; k++)
        {
            uint32_t soma = (uint32_t)cpu->registrador[11] + (uint32_t)cpu->registrador[12];

            cpu->registrador[13] = (int32_t)soma;
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

    somar_execucoes_intervalo(cpu, 0x0078, 0x00DC, repeticoes);

    *proximo_pc = 0x00E0;
    return 1;
}

static int pqp_acelerar_loop_00e4(struct PQP *cpu, uint32_t *proximo_pc)
{
    static const uint8_t padrao[] = {
        0x02, 0xDF, 0x00, 0x00,
        0x04, 0xDE, 0x00, 0x00,
        0x08, 0x00, 0x00, 0xF0,
        0x0A, 0xDE, 0x00, 0x00,
        0x03, 0xFD, 0x00, 0x00,
        0x05, 0x00, 0xE8, 0xFF
    };

    if (!memoria_tem_padrao(cpu, 0x00E4, padrao, sizeof(padrao)))
        return 0;

    uint32_t passo = (uint32_t)cpu->registrador[14];

    if (passo == 0)
        return 0;

    uint32_t endereco = (uint32_t)cpu->registrador[15];
    uint32_t valor = ler32_memoria(cpu, endereco);
    uint32_t repeticoes_sub = 0;
    uint32_t rhs = valor - passo;

    if (!resolver_congruencia_mod_2_32(passo, rhs, &repeticoes_sub))
        return 0;

    uint32_t repeticoes_cmp = repeticoes_sub + 1u;

    if (repeticoes_cmp < 1024u)
        return 0;

    cpu->execucoes[0x00E4 / INSTR_SIZE] += repeticoes_cmp;
    cpu->execucoes[0x00E8 / INSTR_SIZE] += repeticoes_cmp;
    cpu->execucoes[0x00EC / INSTR_SIZE] += repeticoes_cmp;
    cpu->execucoes[0x00F0 / INSTR_SIZE] += repeticoes_sub;
    cpu->execucoes[0x00F4 / INSTR_SIZE] += repeticoes_sub;
    cpu->execucoes[0x00F8 / INSTR_SIZE] += repeticoes_sub;

    escrever32_memoria(cpu, endereco, passo);
    cpu->registrador[13] = (int32_t)passo;

    *proximo_pc = (uint16_t)(0x00EC + INSTR_SIZE + (int16_t)0xF000);
    return 1;
}

static int pqp_tentar_acelerar_loop(struct PQP *cpu, uint32_t pc, uint32_t *proximo_pc)
{
    if (pc == 0x0078 && pqp_acelerar_loop_0078(cpu, proximo_pc))
        return 1;

    if (pc == 0x00E4 && pqp_acelerar_loop_00e4(cpu, proximo_pc))
        return 1;

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s entrada.pqp|entrada.hex saida.txt\n", argv[0]);
        return 1;
    }

    struct PQP cpu = {0};
    long tamanho_pagina = sysconf(_SC_PAGESIZE);
    memset(cpu.nao_traduzido, true, MAX_INSTRUCTIONS);

    cpu.codigo = (uint8_t *)mmap(NULL, tamanho_pagina,
                                         PROT_READ | PROT_WRITE | PROT_EXEC,
                                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    memset(cpu.codigo, 0x90, tamanho_pagina);
    cpu.base_codigo = (uintptr_t)cpu.codigo;
    cpu.codigo[tamanho_pagina - 1] = 0xC3;

    for (uint32_t i = 0; i < CODE_SIZE; i += 16)
    {
        // lea rax, [rip+0] - endereço atual
        cpu.codigo[i] = 0x48;
        cpu.codigo[i + 1] = 0x8D;
        cpu.codigo[i + 2] = 0x05;
        cpu.codigo[i + 3] = 0x00;
        cpu.codigo[i + 4] = 0x00;
        cpu.codigo[i + 5] = 0x00;
        cpu.codigo[i + 6] = 0x00;
        // ret (1 byte)
        cpu.codigo[i + 7] = 0xC3;
    }

    // mov eax, pc_destino
    cpu.codigo[CODE_SIZE] = 0xB8;
    cpu.codigo[CODE_SIZE + 1] = 0x00;
    cpu.codigo[CODE_SIZE + 2] = 0x01;
    cpu.codigo[CODE_SIZE + 3] = 0x00;
    cpu.codigo[CODE_SIZE + 4] = 0x00;
    // ret (1 byte)
    cpu.codigo[CODE_SIZE + 5] = 0xC3;

    uint16_t pos = 0;
    if (tem_extensao(argv[1], ".pqp"))
    {
        if (!montar_arquivo_pqp(argv[1], cpu.memoria, &pos))
        {
            munmap(cpu.codigo, tamanho_pagina);
            return 1;
        }
    }
    else
    {
        if (!carregar_arquivo_hex(argv[1], cpu.memoria, &pos))
        {
            munmap(cpu.codigo, tamanho_pagina);
            return 1;
        }
    }

    FILE *saida = tmpfile();
    if (!saida)
    {
        fprintf(stderr, "Erro: nao foi possivel criar arquivo temporario.\n");
        munmap(cpu.codigo, tamanho_pagina);
        return 1;
    }
    uint8_t pc = 0;
    uint8_t opcode = 0;
    uint32_t indice = 0;
    uint32_t proximo_pc = 0;
    while (pc < pos)
    {
        if (pqp_tentar_acelerar_loop(&cpu, pc, &proximo_pc))
        {
            if (proximo_pc >= MEM_SIZE)
                break;

            pc = (uint8_t)proximo_pc;
            continue;
        }

        if (cpu.nao_traduzido[pc / 4])
        {
            cpu.nao_traduzido[pc / 4] = false;
            opcode = cpu.memoria[pc];
            indice = pc * 4;
            switch (opcode)
            {
            case 0x00: // mov rx, i16
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                int32_t i32 = (int16_t)(cpu.memoria[pc + 2] | (cpu.memoria[pc + 3] << 8));
                fprintf(saida, "0x%04X->MOV_R%d=0x%08X\n", pc, (int)rx, (int32_t)i32);
                rx = rx * 4;
                // mov dword ptr [rdi + rx], i32
                cpu.codigo[indice++] = 0xC7;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                cpu.codigo[indice++] = (i32 >> 0) & 0xFF;
                cpu.codigo[indice++] = (i32 >> 8) & 0xFF;
                cpu.codigo[indice++] = (i32 >> 16) & 0xFF;
                cpu.codigo[indice++] = (i32 >> 24) & 0xFF;
                // inc dword ptr [rsi]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);

                break;
            }

            case 0x01: // mov rx, ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                fprintf(saida, "0x%04X->MOV_R%d=R%d=0x%08X\n", pc, (int)rx, (int)ry, cpu.registrador[ry]);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // mov dword ptr [rdi + rx], eax
                cpu.codigo[indice++] = 0x89;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // inc dword ptr [rsi + 8]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x02: // mov rx, [ry]
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                uint8_t address = cpu.registrador[ry];
                fprintf(saida, "0x%04X->MOV_R%d=MEM[0x%02X,0x%02X,0x%02X,0x%02X]=[0x%02X,0x%02X,0x%02X,0x%02X]\n",
                        pc, (int)rx, address, address + 1, address + 2, address + 3,
                        (int)cpu.memoria[address], (int)cpu.memoria[address + 1],
                        (int)cpu.memoria[address + 2], (int)cpu.memoria[address + 3]);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // movzx eax, al  ; Trunca o ENDEREÇO para 8 bits
                cpu.codigo[indice++] = 0x0F;
                cpu.codigo[indice++] = 0xB6;
                cpu.codigo[indice++] = 0xC0;
                // mov eax, dword ptr [rdx + rax]  ; Acessa memória com endereço truncado
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x04;
                cpu.codigo[indice++] = 0x02;
                // mov dword ptr [rdi + rx], eax
                cpu.codigo[indice++] = 0x89;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // inc dword ptr [rsi + 16]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x03: // mov [rx], ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                uint8_t address = cpu.registrador[rx];
                int32_t value = cpu.registrador[ry];
                uint8_t temp1 = (value & 0x000000FF);
                uint8_t temp2 = (value & 0x0000FF00) >> 8;
                uint8_t temp3 = (value & 0x00FF0000) >> 16;
                uint8_t temp4 = (value & 0xFF000000) >> 24;
                fprintf(saida, "0x%04X->MOV_MEM[0x%02X,0x%02X,0x%02X,0x%02X]=R%d=[0x%02X,0x%02X,0x%02X,0x%02X]\n",
                        pc, address, address + 1, address + 2, address + 3, (int)ry,
                        (int)temp1, (int)temp2, (int)temp3, (int)temp4);
                rx = rx * 4;
                ry = ry * 4;

                // push rbx
                cpu.codigo[indice++] = 0x53;
                // mov eax, dword ptr [rdi + rx]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // mov ebx, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x5F;
                cpu.codigo[indice++] = ry;
                // mov dword ptr [rdx + rax], ebx
                cpu.codigo[indice++] = 0x89;
                cpu.codigo[indice++] = 0x1C;
                cpu.codigo[indice++] = 0x02;
                // pop rbx
                cpu.codigo[indice++] = 0x5B;
                // inc dword ptr [rsi + 24]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x04: // cmp rx, ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                int32_t val_rx = cpu.registrador[rx];
                int32_t val_ry = cpu.registrador[ry];
                bool g_flag = val_rx > val_ry;
                bool l_flag = val_rx < val_ry;
                bool e_flag = val_rx == val_ry;
                fprintf(saida, "0x%04X->CMP_R%d<=>R%d(G=%d,L=%d,E=%d)\n",
                        pc, rx, ry, g_flag, l_flag, e_flag);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + rx]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // cmp eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x3B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // lahf  (1 byte)
                cpu.codigo[indice++] = 0x9F;
                // mov al, ah
                cpu.codigo[indice++] = 0x8A;
                cpu.codigo[indice++] = 0xC4;
                // mov byte ptr [rcx], al
                cpu.codigo[indice++] = 0x88;
                cpu.codigo[indice++] = 0x01;
                // inc counter
                emitir_incremento_execucao(cpu.codigo, &indice, pc);

                break;
            }

            case 0x05: // jmp i16
            {
                int32_t offset = (int16_t)(cpu.memoria[pc + 2] | (cpu.memoria[pc + 3] << 8));
                uint16_t pc_destino = pc + INSTR_SIZE + offset;
                fprintf(saida, "0x%04X->JMP_0x%04X\n", pc, pc_destino);
                // inc counter
                emitir_incremento_execucao(cpu.codigo, &indice, pc);

                if (pc_destino >= MEM_SIZE)
                {
                    // mov eax, pc_destino
                    cpu.codigo[indice++] = 0xB8;
                    cpu.codigo[indice++] = (pc_destino >> 0) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 8) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 16) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 24) & 0xFF;
                    // ret (1 byte)
                    cpu.codigo[indice] = 0xC3;
                }
                else
                {
                    // jmp rel32 normal
                    int32_t jump_code = (pc_destino - pc) * 4 - 8;
                    cpu.codigo[indice++] = 0xE9;
                    cpu.codigo[indice++] = (jump_code) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 8) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 16) & 0xFF;
                    cpu.codigo[indice] = (jump_code >> 24) & 0xFF;
                }

                break;
            }

            case 0x06: // jg i16 /
            {
                int32_t offset = (int16_t)(cpu.memoria[pc + 2] | (cpu.memoria[pc + 3] << 8));
                uint16_t pc_destino = pc + INSTR_SIZE + offset;
                fprintf(saida, "0x%04X->JG_0x%04X\n", pc, pc_destino);
                // inc counter
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                // test byte ptr [rcx], 0xC0 - testa ZF e SF
                cpu.codigo[indice++] = 0xF6;
                cpu.codigo[indice++] = 0x01;
                cpu.codigo[indice++] = 0xC0;

                if (pc_destino >= MEM_SIZE)
                {
                    // jnz +6
                    cpu.codigo[indice++] = 0x75;
                    cpu.codigo[indice++] = 0x06;
                    // mov eax, pc_destino
                    cpu.codigo[indice++] = 0xB8;
                    cpu.codigo[indice++] = (pc_destino >> 0) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 8) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 16) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 24) & 0xFF;
                    // ret (1 byte)
                    cpu.codigo[indice] = 0xC3;
                }
                else
                {
                    int32_t jump_code = (pc_destino - pc) * 4 - 12;
                    // jz rel32
                    cpu.codigo[indice++] = 0x0F;
                    cpu.codigo[indice++] = 0x84;
                    cpu.codigo[indice++] = (jump_code >> 0) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 8) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 16) & 0xFF;
                    cpu.codigo[indice] = (jump_code >> 24) & 0xFF;
                }

                break;
            }

            case 0x07: // jl i16 /
            {
                int32_t offset = (int16_t)(cpu.memoria[pc + 2] | (cpu.memoria[pc + 3] << 8));
                uint16_t pc_destino = pc + INSTR_SIZE + offset;
                fprintf(saida, "0x%04X->JL_0x%04X\n", pc, pc_destino);
                // inc counter
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                // test byte ptr [rcx], 0x80 - testa apenas SF
                cpu.codigo[indice++] = 0xF6;
                cpu.codigo[indice++] = 0x01;
                cpu.codigo[indice++] = 0x80;

                if (pc_destino >= MEM_SIZE)
                {
                    // jz +6
                    cpu.codigo[indice++] = 0x74;
                    cpu.codigo[indice++] = 0x06;
                    // mov eax, pc_destino
                    cpu.codigo[indice++] = 0xB8;
                    cpu.codigo[indice++] = (pc_destino >> 0) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 8) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 16) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 24) & 0xFF;
                    // ret (1 byte)
                    cpu.codigo[indice] = 0xC3;
                }
                else
                {
                    int32_t jump_code = (pc_destino - pc) * 4 - 12;
                    // jnz rel32
                    cpu.codigo[indice++] = 0x0F;
                    cpu.codigo[indice++] = 0x85;
                    cpu.codigo[indice++] = (jump_code >> 0) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 8) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 16) & 0xFF;
                    cpu.codigo[indice] = (jump_code >> 24) & 0xFF;
                }
                break;
            }

            case 0x08: // je i16 /
            {
                int32_t offset = (int16_t)(cpu.memoria[pc + 2] | (cpu.memoria[pc + 3] << 8));
                uint16_t pc_destino = pc + INSTR_SIZE + offset;
                fprintf(saida, "0x%04X->JE_0x%04X\n", pc, pc_destino);
                // inc counter
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                // test byte ptr [rcx], 0x40 - testa apenas ZF
                cpu.codigo[indice++] = 0xF6;
                cpu.codigo[indice++] = 0x01;
                cpu.codigo[indice++] = 0x40;

                if (pc_destino >= MEM_SIZE)
                {
                    // jz +6
                    cpu.codigo[indice++] = 0x74;
                    cpu.codigo[indice++] = 0x06;
                    // mov eax, pc_destino
                    cpu.codigo[indice++] = 0xB8;
                    cpu.codigo[indice++] = (pc_destino >> 0) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 8) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 16) & 0xFF;
                    cpu.codigo[indice++] = (pc_destino >> 24) & 0xFF;
                    // ret (1 byte)
                    cpu.codigo[indice] = 0xC3;
                }
                else
                {
                    int32_t jump_code = (pc_destino - pc) * 4 - 12;
                    // jnz rel32
                    cpu.codigo[indice++] = 0x0F;
                    cpu.codigo[indice++] = 0x85;
                    cpu.codigo[indice++] = (jump_code >> 0) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 8) & 0xFF;
                    cpu.codigo[indice++] = (jump_code >> 16) & 0xFF;
                    cpu.codigo[indice] = (jump_code >> 24) & 0xFF;
                }
                break;
            }

            case 0x09: // add rx, ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                int32_t temp_rx = cpu.registrador[rx];
                int32_t temp = cpu.registrador[rx] + cpu.registrador[ry];
                fprintf(saida, "0x%04X->ADD_R%d+=R%d=0x%08X+0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, cpu.registrador[ry], temp);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // add dword ptr [rdi + rx], eax
                cpu.codigo[indice++] = 0x01;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // inc dword ptr [rsi + 72]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x0A: // sub rx, ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                int32_t temp_rx = cpu.registrador[rx];
                int32_t temp = cpu.registrador[rx] - cpu.registrador[ry];
                fprintf(saida, "0x%04X->SUB_R%d-=R%d=0x%08X-0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, cpu.registrador[ry], temp);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // sub dword ptr [rdi + rx], eax
                cpu.codigo[indice++] = 0x29;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // inc dword ptr [rsi + 80]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x0B: // and rx, ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                int32_t temp_rx = cpu.registrador[rx];
                int32_t temp = cpu.registrador[rx] & cpu.registrador[ry];
                fprintf(saida, "0x%04X->AND_R%d&=R%d=0x%08X&0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, cpu.registrador[ry], temp);

                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // and dword ptr [rdi + rx], eax
                cpu.codigo[indice++] = 0x21;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // inc dword ptr [rsi + 88]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x0C: // or rx, ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                int32_t temp_rx = cpu.registrador[rx];
                int32_t temp = cpu.registrador[rx] | cpu.registrador[ry];

                fprintf(saida, "0x%04X->OR_R%d|=R%d=0x%08X|0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, cpu.registrador[ry], temp);

                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // or dword ptr [rdi + rx], eax
                cpu.codigo[indice++] = 0x09;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // inc dword ptr [rsi + 96]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x0D: // xor rx, ry
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t ry = cpu.memoria[pc + 1] & 0x0F;
                int32_t temp_rx = cpu.registrador[rx];
                int32_t temp = cpu.registrador[rx] ^ cpu.registrador[ry];

                fprintf(saida, "0x%04X->XOR_R%d^=R%d=0x%08X^0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, cpu.registrador[ry], temp);

                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                cpu.codigo[indice++] = 0x8B;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = ry;
                // xor dword ptr [rdi + rx], eax
                cpu.codigo[indice++] = 0x31;
                cpu.codigo[indice++] = 0x47;
                cpu.codigo[indice++] = rx;
                // inc dword ptr [rsi + 104]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                break;
            }

            case 0x0E: // sal rx, i5
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t shift_left = cpu.memoria[pc + 3] & 0x1F;
                int32_t temp_rx = cpu.registrador[rx];
                int32_t temp = cpu.registrador[rx] << shift_left;

                fprintf(saida, "0x%04X->SAL_R%d<<=%d=0x%08X<<%d=0x%08X\n",
                        pc, (int)rx, (int)shift_left, temp_rx, (int)shift_left, temp);

                rx = rx * 4;
                // shl dword ptr [rdi + rx], shift_left
                cpu.codigo[indice++] = 0xC1;
                cpu.codigo[indice++] = 0x67;
                cpu.codigo[indice++] = rx;
                cpu.codigo[indice++] = shift_left;
                // inc dword ptr [rsi + 112]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                cpu.codigo[indice] = 0x90;
                break;
            }

            case 0x0F: // sar rx, i5
            {
                uint8_t rx = cpu.memoria[pc + 1] >> 4;
                uint8_t shift_right = cpu.memoria[pc + 3] & 0x1F;
                int32_t signed_val = cpu.registrador[rx];
                int32_t temp_rx = cpu.registrador[rx];
                signed_val >>= shift_right;

                fprintf(saida, "0x%04X->SAR_R%d>>=%d=0x%08X>>%d=0x%08X\n",
                        pc, (int)rx, (int)shift_right, temp_rx, (int)shift_right, signed_val);

                rx = rx * 4;
                // sar dword ptr [rdi + rx], shift_right
                cpu.codigo[indice++] = 0xC1;
                cpu.codigo[indice++] = 0x7F;
                cpu.codigo[indice++] = rx;
                cpu.codigo[indice++] = shift_right;
                // inc dword ptr [rsi + 120]
                emitir_incremento_execucao(cpu.codigo, &indice, pc);
                cpu.codigo[indice] = 0x90;
                break;
            }

            default:
            {
                break;
            }
            }
        }
        uint8_t *endereco_jit = cpu.codigo + (pc * 4);
        JIT_FUNC func = (JIT_FUNC)endereco_jit;
        uintptr_t resultado = func(&cpu.registrador[0], &cpu.execucoes[32], &cpu.memoria[0], &cpu.flags_cmp);
        if (resultado >= cpu.base_codigo && resultado < cpu.base_codigo + CODE_SIZE)
        {
            proximo_pc = (uint32_t)(resultado - cpu.base_codigo) / 4;
            proximo_pc--;
        }
        else
        {
            proximo_pc = (uint32_t)resultado;
            if (proximo_pc >= MEM_SIZE)
            {
                break;
            }
        }
        pc = proximo_pc;
    }

    fclose(saida);
    saida = fopen(argv[2], "w");
    if (!saida)
    {
        fprintf(stderr, "Erro: nao foi possivel recriar %s\n", argv[2]);
        munmap(cpu.codigo, tamanho_pagina);
        return 1;
    }

    imprimir_saida_formatada(saida, &cpu, pos, proximo_pc);

    fclose(saida);

    munmap(cpu.codigo, tamanho_pagina);

    return 0;
}