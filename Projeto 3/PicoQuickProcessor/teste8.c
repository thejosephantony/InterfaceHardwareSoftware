#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>

#define REGISTERS_NUM 16
#define MEMORY_SIZE 256
#define INTERPRETED_SIZE 64
#define INSTRUCTION_SIZE 4
#define SIZE_CODE 1024

typedef uintptr_t (*JitFunc)(int32_t *, uint32_t *, uint8_t *, uint32_t *);

struct Machine_x86
{
    int32_t registers[REGISTERS_NUM];
    uint8_t memory[MEMORY_SIZE];
    uint32_t save_bool;
    uint32_t instruction_counts[INTERPRETED_SIZE];
    bool not_interpreted[INTERPRETED_SIZE];

    uint8_t *executable_code;
    uintptr_t code_base;
};



#define MAX_LINES 256
#define MAX_LINE_LEN 128
#define MAX_LABELS 128

struct Label
{
    char name[64];
    uint16_t pc;
};

static void trim(char *s)
{
    size_t len;
    while (isspace((unsigned char)*s))
        memmove(s, s + 1, strlen(s));

    len = strlen(s);
    while (len > 0 && isspace((unsigned char)s[len - 1]))
        s[--len] = '\0';
}

static void remove_comment(char *s)
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

static void upper_string(char *s)
{
    for (; *s; s++)
        *s = (char)toupper((unsigned char)*s);
}

static int parse_register(const char *s)
{
    if (s == NULL || toupper((unsigned char)s[0]) != 'R')
        return -1;

    char *end = NULL;
    long value = strtol(s + 1, &end, 10);

    if (end == s + 1 || *end != '\0' || value < 0 || value > 15)
        return -1;

    return (int)value;
}

static int parse_memory_register(const char *s)
{
    char temp[32];
    size_t len;

    if (s == NULL)
        return -1;

    snprintf(temp, sizeof(temp), "%s", s);
    trim(temp);
    len = strlen(temp);

    if (len < 4 || temp[0] != '[' || temp[len - 1] != ']')
        return -1;

    temp[len - 1] = '\0';
    return parse_register(temp + 1);
}

static int find_label(struct Label *labels, int label_count, const char *name)
{
    for (int i = 0; i < label_count; i++)
    {
        if (strcmp(labels[i].name, name) == 0)
            return labels[i].pc;
    }
    return -1;
}

static void emit_rr(uint8_t *memory, uint16_t *pos, uint8_t opcode, int rx, int ry)
{
    memory[(*pos)++] = opcode;
    memory[(*pos)++] = (uint8_t)((rx << 4) | ry);
    memory[(*pos)++] = 0x00;
    memory[(*pos)++] = 0x00;
}

static void emit_ri16(uint8_t *memory, uint16_t *pos, uint8_t opcode, int rx, int16_t imm)
{
    memory[(*pos)++] = opcode;
    memory[(*pos)++] = (uint8_t)(rx << 4);
    memory[(*pos)++] = (uint8_t)(imm & 0xFF);
    memory[(*pos)++] = (uint8_t)((imm >> 8) & 0xFF);
}

static void emit_jump(uint8_t *memory, uint16_t *pos, uint8_t opcode, int16_t offset)
{
    memory[(*pos)++] = opcode;
    memory[(*pos)++] = 0x00;
    memory[(*pos)++] = (uint8_t)(offset & 0xFF);
    memory[(*pos)++] = (uint8_t)((offset >> 8) & 0xFF);
}

static int split_operands(char *args, char **a, char **b)
{
    char *comma = strchr(args, ',');
    if (!comma)
        return 0;

    *comma = '\0';
    *a = args;
    *b = comma + 1;
    trim(*a);
    trim(*b);
    return 1;
}

static int assemble_instruction(char *line, uint8_t *memory, uint16_t *pos, struct Label *labels, int label_count)
{
    char work[MAX_LINE_LEN];
    char *mnemonic;
    char *args;
    char *op1;
    char *op2;
    uint16_t current_pc = *pos;

    snprintf(work, sizeof(work), "%s", line);
    trim(work);
    upper_string(work);

    if (work[0] == '\0')
        return 1;

    mnemonic = strtok(work, " \t");
    args = strtok(NULL, "");

    if (!mnemonic)
        return 1;

    if (strcmp(mnemonic, "MOV") == 0)
    {
        if (!args || !split_operands(args, &op1, &op2))
            return 0;

        int mem_rx = parse_memory_register(op1);
        int mem_ry = parse_memory_register(op2);
        int rx = parse_register(op1);
        int ry = parse_register(op2);

        if (rx >= 0 && ry >= 0)
        {
            emit_rr(memory, pos, 0x01, rx, ry);
            return 1;
        }
        if (rx >= 0 && mem_ry >= 0)
        {
            emit_rr(memory, pos, 0x02, rx, mem_ry);
            return 1;
        }
        if (mem_rx >= 0 && ry >= 0)
        {
            emit_rr(memory, pos, 0x03, mem_rx, ry);
            return 1;
        }
        if (rx >= 0)
        {
            char *end = NULL;
            long imm = strtol(op2, &end, 0);
            if (end != op2 && *end == '\0' && imm >= -32768 && imm <= 32767)
            {
                emit_ri16(memory, pos, 0x00, rx, (int16_t)imm);
                return 1;
            }
        }
        return 0;
    }

    if (strcmp(mnemonic, "CMP") == 0 || strcmp(mnemonic, "ADD") == 0 ||
        strcmp(mnemonic, "SUB") == 0 || strcmp(mnemonic, "AND") == 0 ||
        strcmp(mnemonic, "OR") == 0 || strcmp(mnemonic, "XOR") == 0)
    {
        uint8_t opcode = 0x04;
        if (strcmp(mnemonic, "ADD") == 0) opcode = 0x09;
        else if (strcmp(mnemonic, "SUB") == 0) opcode = 0x0A;
        else if (strcmp(mnemonic, "AND") == 0) opcode = 0x0B;
        else if (strcmp(mnemonic, "OR") == 0) opcode = 0x0C;
        else if (strcmp(mnemonic, "XOR") == 0) opcode = 0x0D;

        if (!args || !split_operands(args, &op1, &op2))
            return 0;

        int rx = parse_register(op1);
        int ry = parse_register(op2);
        if (rx < 0 || ry < 0)
            return 0;

        emit_rr(memory, pos, opcode, rx, ry);
        return 1;
    }

    if (strcmp(mnemonic, "SAL") == 0 || strcmp(mnemonic, "SAR") == 0)
    {
        if (!args || !split_operands(args, &op1, &op2))
            return 0;

        int rx = parse_register(op1);
        char *end = NULL;
        long shift = strtol(op2, &end, 0);

        if (rx < 0 || end == op2 || *end != '\0' || shift < 0 || shift > 31)
            return 0;

        memory[(*pos)++] = (strcmp(mnemonic, "SAL") == 0) ? 0x0E : 0x0F;
        memory[(*pos)++] = (uint8_t)(rx << 4);
        memory[(*pos)++] = 0x00;
        memory[(*pos)++] = (uint8_t)shift;
        return 1;
    }

    if (strcmp(mnemonic, "JMP") == 0 || strcmp(mnemonic, "JG") == 0 ||
        strcmp(mnemonic, "JL") == 0 || strcmp(mnemonic, "JE") == 0)
    {
        uint8_t opcode = 0x05;
        int target_pc;
        char *end = NULL;
        long direct_offset;

        if (strcmp(mnemonic, "JG") == 0) opcode = 0x06;
        else if (strcmp(mnemonic, "JL") == 0) opcode = 0x07;
        else if (strcmp(mnemonic, "JE") == 0) opcode = 0x08;

        if (!args)
            return 0;
        trim(args);

        target_pc = find_label(labels, label_count, args);
        if (target_pc >= 0)
        {
            int32_t offset = target_pc - (int32_t)(current_pc + INSTRUCTION_SIZE);
            if (offset < -32768 || offset > 32767)
                return 0;
            emit_jump(memory, pos, opcode, (int16_t)offset);
            return 1;
        }

        direct_offset = strtol(args, &end, 0);
        if (end != args && *end == '\0' && direct_offset >= -32768 && direct_offset <= 32767)
        {
            emit_jump(memory, pos, opcode, (int16_t)direct_offset);
            return 1;
        }

        return 0;
    }

    return 0;
}

static int assemble_pqp_file(const char *filename, uint8_t *memory, uint16_t *pos)
{
    FILE *input = fopen(filename, "r");
    char lines[MAX_LINES][MAX_LINE_LEN];
    int line_count = 0;
    struct Label labels[MAX_LABELS];
    int label_count = 0;
    uint16_t pc = 0;

    if (!input)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", filename);
        return 0;
    }

    while (line_count < MAX_LINES && fgets(lines[line_count], MAX_LINE_LEN, input))
    {
        remove_comment(lines[line_count]);
        trim(lines[line_count]);
        line_count++;
    }
    fclose(input);

    for (int i = 0; i < line_count; i++)
    {
        char temp[MAX_LINE_LEN];
        char *colon;
        snprintf(temp, sizeof(temp), "%s", lines[i]);
        trim(temp);
        if (temp[0] == '\0')
            continue;

        colon = strchr(temp, ':');
        if (colon)
        {
            *colon = '\0';
            trim(temp);
            upper_string(temp);
            if (label_count >= MAX_LABELS)
            {
                fprintf(stderr, "Erro: muitos rotulos.\n");
                return 0;
            }
            snprintf(labels[label_count].name, sizeof(labels[label_count].name), "%s", temp);
            labels[label_count].pc = pc;
            label_count++;

            colon++;
            trim(colon);
            if (*colon != '\0')
                pc += INSTRUCTION_SIZE;
        }
        else
        {
            pc += INSTRUCTION_SIZE;
        }
    }

    *pos = 0;
    for (int i = 0; i < line_count; i++)
    {
        char temp[MAX_LINE_LEN];
        char *colon;
        snprintf(temp, sizeof(temp), "%s", lines[i]);
        trim(temp);
        if (temp[0] == '\0')
            continue;

        colon = strchr(temp, ':');
        if (colon)
        {
            colon++;
            trim(colon);
            if (*colon == '\0')
                continue;
            snprintf(temp, sizeof(temp), "%s", colon);
        }

        if (*pos + INSTRUCTION_SIZE > MEMORY_SIZE)
        {
            fprintf(stderr, "Erro: programa maior que a memoria da PQP.\n");
            return 0;
        }

        if (!assemble_instruction(temp, memory, pos, labels, label_count))
        {
            fprintf(stderr, "Erro de sintaxe na linha %d: %s\n", i + 1, lines[i]);
            return 0;
        }
    }

    return 1;
}

static int read_hex_file(const char *filename, uint8_t *memory, uint16_t *pos)
{
    FILE *input = fopen(filename, "r");
    uint8_t hex_value;

    if (!input)
    {
        fprintf(stderr, "Erro: nao foi possivel abrir %s\n", filename);
        return 0;
    }

    *pos = 0;
    while (fscanf(input, "%hhx", &hex_value) == 1)
    {
        if (*pos >= MEMORY_SIZE)
        {
            fclose(input);
            fprintf(stderr, "Erro: arquivo hexadecimal maior que a memoria da PQP.\n");
            return 0;
        }
        memory[(*pos)++] = hex_value;
    }
    fclose(input);
    return 1;
}

static int has_extension(const char *filename, const char *extension)
{
    size_t lf = strlen(filename);
    size_t le = strlen(extension);
    if (lf < le)
        return 0;
    return strcmp(filename + lf - le, extension) == 0;
}

static void format_instruction(const uint8_t *memory, uint16_t pc, char *out, size_t out_size)
{
    uint8_t opcode = memory[pc];
    uint8_t rx = memory[pc + 1] >> 4;
    uint8_t ry = memory[pc + 1] & 0x0F;
    int32_t imm = (int16_t)(memory[pc + 2] | (memory[pc + 3] << 8));
    uint8_t shift = memory[pc + 3] & 0x1F;
    uint16_t target = (uint16_t)(pc + INSTRUCTION_SIZE + (int16_t)(memory[pc + 2] | (memory[pc + 3] << 8)));

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

static void print_formatted_output(FILE *output, const struct Machine_x86 *vm, uint16_t pos, uint32_t exit_pc)
{
    char text[64];

    for (uint16_t pc = 0; pc < pos; pc += INSTRUCTION_SIZE)
    {
        format_instruction(vm->memory, pc, text, sizeof(text));
        fprintf(output, "%13u:%04x:%s\n", vm->instruction_counts[pc / 4], pc, text);
    }

    fprintf(output, "%13s:%04x:exit\n\n", "-", (uint16_t)exit_pc);

    for (int i = 0; i < REGISTERS_NUM; i++)
    {
        fprintf(output, "r%d=%08x", i, (uint32_t)vm->registers[i]);
        if (i < REGISTERS_NUM - 1)
            fprintf(output, ",");
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Uso: %s entrada.pqp|entrada.hex saida.txt\n", argv[0]);
        return 1;
    }

    struct Machine_x86 vm = {0};
    long page_size = sysconf(_SC_PAGESIZE);
    memset(vm.not_interpreted, true, INTERPRETED_SIZE);

    vm.executable_code = (uint8_t *)mmap(NULL, page_size,
                                         PROT_READ | PROT_WRITE | PROT_EXEC,
                                         MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    memset(vm.executable_code, 0x90, page_size);
    vm.code_base = (uintptr_t)vm.executable_code;
    vm.executable_code[page_size - 1] = 0xC3;

    for (uint32_t i = 0; i < SIZE_CODE; i += 16)
    {
        // lea rax, [rip+0] - endereço atual
        vm.executable_code[i] = 0x48;
        vm.executable_code[i + 1] = 0x8D;
        vm.executable_code[i + 2] = 0x05;
        vm.executable_code[i + 3] = 0x00;
        vm.executable_code[i + 4] = 0x00;
        vm.executable_code[i + 5] = 0x00;
        vm.executable_code[i + 6] = 0x00;
        // ret (1 byte)
        vm.executable_code[i + 7] = 0xC3;
    }

    // mov eax, target_pc
    vm.executable_code[SIZE_CODE] = 0xB8;
    vm.executable_code[SIZE_CODE + 1] = 0x00;
    vm.executable_code[SIZE_CODE + 2] = 0x01;
    vm.executable_code[SIZE_CODE + 3] = 0x00;
    vm.executable_code[SIZE_CODE + 4] = 0x00;
    // ret (1 byte)
    vm.executable_code[SIZE_CODE + 5] = 0xC3;

    uint16_t pos = 0;
    if (has_extension(argv[1], ".pqp"))
    {
        if (!assemble_pqp_file(argv[1], vm.memory, &pos))
        {
            munmap(vm.executable_code, page_size);
            return 1;
        }
    }
    else
    {
        if (!read_hex_file(argv[1], vm.memory, &pos))
        {
            munmap(vm.executable_code, page_size);
            return 1;
        }
    }

    FILE *output = fopen(argv[2], "w");
    if (!output)
    {
        fprintf(stderr, "Erro: nao foi possivel criar %s\n", argv[2]);
        munmap(vm.executable_code, page_size);
        return 1;
    }
    uint8_t pc = 0;
    uint8_t opcode = 0;
    uint32_t index = 0;
    uint32_t temp_pc = 0;
    while (pc < pos)
    {
        if (vm.not_interpreted[pc / 4])
        {
            vm.not_interpreted[pc / 4] = false;
            opcode = vm.memory[pc];
            index = pc * 4;
            switch (opcode)
            {
            case 0x00: // mov rx, i16
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                int32_t i32 = (int16_t)(vm.memory[pc + 2] | (vm.memory[pc + 3] << 8));
                fprintf(output, "0x%04X->MOV_R%d=0x%08X\n", pc, (int)rx, (int32_t)i32);
                rx = rx * 4;
                // mov dword ptr [rdi + rx], i32
                vm.executable_code[index++] = 0xC7;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                vm.executable_code[index++] = (i32 >> 0) & 0xFF;
                vm.executable_code[index++] = (i32 >> 8) & 0xFF;
                vm.executable_code[index++] = (i32 >> 16) & 0xFF;
                vm.executable_code[index++] = (i32 >> 24) & 0xFF;
                // inc dword ptr [rsi]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index] = 0x06;

                break;
            }

            case 0x01: // mov rx, ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                fprintf(output, "0x%04X->MOV_R%d=R%d=0x%08X\n", pc, (int)rx, (int)ry, vm.registers[ry]);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // mov dword ptr [rdi + rx], eax
                vm.executable_code[index++] = 0x89;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // inc dword ptr [rsi + 8]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x02: // mov rx, [ry]
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                uint8_t address = vm.registers[ry];
                fprintf(output, "0x%04X->MOV_R%d=MEM[0x%02X,0x%02X,0x%02X,0x%02X]=[0x%02X,0x%02X,0x%02X,0x%02X]\n",
                        pc, (int)rx, address, address + 1, address + 2, address + 3,
                        (int)vm.memory[address], (int)vm.memory[address + 1],
                        (int)vm.memory[address + 2], (int)vm.memory[address + 3]);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // movzx eax, al  ; Trunca o ENDEREÇO para 8 bits
                vm.executable_code[index++] = 0x0F;
                vm.executable_code[index++] = 0xB6;
                vm.executable_code[index++] = 0xC0;
                // mov eax, dword ptr [rdx + rax]  ; Acessa memória com endereço truncado
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x04;
                vm.executable_code[index++] = 0x02;
                // mov dword ptr [rdi + rx], eax
                vm.executable_code[index++] = 0x89;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // inc dword ptr [rsi + 16]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x03: // mov [rx], ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                uint8_t address = vm.registers[rx];
                int32_t value = vm.registers[ry];
                uint8_t temp1 = (value & 0x000000FF);
                uint8_t temp2 = (value & 0x0000FF00) >> 8;
                uint8_t temp3 = (value & 0x00FF0000) >> 16;
                uint8_t temp4 = (value & 0xFF000000) >> 24;
                fprintf(output, "0x%04X->MOV_MEM[0x%02X,0x%02X,0x%02X,0x%02X]=R%d=[0x%02X,0x%02X,0x%02X,0x%02X]\n",
                        pc, address, address + 1, address + 2, address + 3, (int)ry,
                        (int)temp1, (int)temp2, (int)temp3, (int)temp4);
                rx = rx * 4;
                ry = ry * 4;

                // push rbx
                vm.executable_code[index++] = 0x53;
                // mov eax, dword ptr [rdi + rx]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // mov ebx, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x5F;
                vm.executable_code[index++] = ry;
                // mov dword ptr [rdx + rax], ebx
                vm.executable_code[index++] = 0x89;
                vm.executable_code[index++] = 0x1C;
                vm.executable_code[index++] = 0x02;
                // pop rbx
                vm.executable_code[index++] = 0x5B;
                // inc dword ptr [rsi + 24]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x04: // cmp rx, ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                int32_t val_rx = vm.registers[rx];
                int32_t val_ry = vm.registers[ry];
                bool g_flag = val_rx > val_ry;
                bool l_flag = val_rx < val_ry;
                bool e_flag = val_rx == val_ry;
                fprintf(output, "0x%04X->CMP_R%d<=>R%d(G=%d,L=%d,E=%d)\n",
                        pc, rx, ry, g_flag, l_flag, e_flag);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + rx]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // cmp eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x3B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // lahf  (1 byte)
                vm.executable_code[index++] = 0x9F;
                // mov al, ah
                vm.executable_code[index++] = 0x8A;
                vm.executable_code[index++] = 0xC4;
                // mov byte ptr [rcx], al
                vm.executable_code[index++] = 0x88;
                vm.executable_code[index++] = 0x01;
                // inc counter
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;

                break;
            }

            case 0x05: // jmp i16
            {
                int32_t offset = (int32_t)(vm.memory[pc + 2] | (vm.memory[pc + 3] << 8));
                uint16_t target_pc = pc + INSTRUCTION_SIZE + offset;
                fprintf(output, "0x%04X->JMP_0x%04X\n", pc, target_pc);
                // inc counter
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index++] = 0x90;

                if (target_pc >= MEMORY_SIZE)
                {
                    // mov eax, target_pc
                    vm.executable_code[index++] = 0xB8;
                    vm.executable_code[index++] = (target_pc >> 0) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 8) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 16) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 24) & 0xFF;
                    // ret (1 byte)
                    vm.executable_code[index] = 0xC3;
                }
                else
                {
                    // jmp rel32 normal
                    int32_t jump_code = (target_pc - pc) * 4 - 8;
                    vm.executable_code[index++] = 0xE9;
                    vm.executable_code[index++] = (jump_code) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 8) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 16) & 0xFF;
                    vm.executable_code[index] = (jump_code >> 24) & 0xFF;
                }

                break;
            }

            case 0x06: // jg i16 /
            {
                int32_t offset = (int32_t)(vm.memory[pc + 2] | (vm.memory[pc + 3] << 8));
                uint16_t target_pc = pc + INSTRUCTION_SIZE + offset;
                fprintf(output, "0x%04X->JG_0x%04X\n", pc, target_pc);
                // inc counter
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index++] = 0x90;
                // test byte ptr [rcx], 0xC0 - testa ZF e SF
                vm.executable_code[index++] = 0xF6;
                vm.executable_code[index++] = 0x01;
                vm.executable_code[index++] = 0xC0;

                if (target_pc >= MEMORY_SIZE)
                {
                    // jnz +6
                    vm.executable_code[index++] = 0x75;
                    vm.executable_code[index++] = 0x06;
                    // mov eax, target_pc
                    vm.executable_code[index++] = 0xB8;
                    vm.executable_code[index++] = (target_pc >> 0) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 8) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 16) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 24) & 0xFF;
                    // ret (1 byte)
                    vm.executable_code[index] = 0xC3;
                }
                else
                {
                    int32_t jump_code = (target_pc - pc) * 4 - 12;
                    // jz rel32
                    vm.executable_code[index++] = 0x0F;
                    vm.executable_code[index++] = 0x84;
                    vm.executable_code[index++] = (jump_code >> 0) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 8) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 16) & 0xFF;
                    vm.executable_code[index] = (jump_code >> 24) & 0xFF;
                }

                break;
            }

            case 0x07: // jl i16 /
            {
                int32_t offset = (int32_t)(vm.memory[pc + 2] | (vm.memory[pc + 3] << 8));
                uint16_t target_pc = pc + INSTRUCTION_SIZE + offset;
                fprintf(output, "0x%04X->JL_0x%04X\n", pc, target_pc);
                // inc counter
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index++] = 0x90;
                // test byte ptr [rcx], 0x80 - testa apenas SF
                vm.executable_code[index++] = 0xF6;
                vm.executable_code[index++] = 0x01;
                vm.executable_code[index++] = 0x80;

                if (target_pc >= MEMORY_SIZE)
                {
                    // jz +6
                    vm.executable_code[index++] = 0x74;
                    vm.executable_code[index++] = 0x06;
                    // mov eax, target_pc
                    vm.executable_code[index++] = 0xB8;
                    vm.executable_code[index++] = (target_pc >> 0) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 8) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 16) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 24) & 0xFF;
                    // ret (1 byte)
                    vm.executable_code[index] = 0xC3;
                }
                else
                {
                    int32_t jump_code = (target_pc - pc) * 4 - 12;
                    // jnz rel32
                    vm.executable_code[index++] = 0x0F;
                    vm.executable_code[index++] = 0x85;
                    vm.executable_code[index++] = (jump_code >> 0) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 8) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 16) & 0xFF;
                    vm.executable_code[index] = (jump_code >> 24) & 0xFF;
                }
                break;
            }

            case 0x08: // je i16 /
            {
                int32_t offset = (int32_t)(vm.memory[pc + 2] | (vm.memory[pc + 3] << 8));
                uint16_t target_pc = pc + INSTRUCTION_SIZE + offset;
                fprintf(output, "0x%04X->JE_0x%04X\n", pc, target_pc);
                // inc counter
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index++] = 0x90;
                // test byte ptr [rcx], 0x40 - testa apenas ZF
                vm.executable_code[index++] = 0xF6;
                vm.executable_code[index++] = 0x01;
                vm.executable_code[index++] = 0x40;

                if (target_pc >= MEMORY_SIZE)
                {
                    // jz +6
                    vm.executable_code[index++] = 0x74;
                    vm.executable_code[index++] = 0x06;
                    // mov eax, target_pc
                    vm.executable_code[index++] = 0xB8;
                    vm.executable_code[index++] = (target_pc >> 0) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 8) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 16) & 0xFF;
                    vm.executable_code[index++] = (target_pc >> 24) & 0xFF;
                    // ret (1 byte)
                    vm.executable_code[index] = 0xC3;
                }
                else
                {
                    int32_t jump_code = (target_pc - pc) * 4 - 12;
                    // jnz rel32
                    vm.executable_code[index++] = 0x0F;
                    vm.executable_code[index++] = 0x85;
                    vm.executable_code[index++] = (jump_code >> 0) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 8) & 0xFF;
                    vm.executable_code[index++] = (jump_code >> 16) & 0xFF;
                    vm.executable_code[index] = (jump_code >> 24) & 0xFF;
                }
                break;
            }

            case 0x09: // add rx, ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                int32_t temp_rx = vm.registers[rx];
                int32_t temp = vm.registers[rx] + vm.registers[ry];
                fprintf(output, "0x%04X->ADD_R%d+=R%d=0x%08X+0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, vm.registers[ry], temp);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // add dword ptr [rdi + rx], eax
                vm.executable_code[index++] = 0x01;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // inc dword ptr [rsi + 72]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x0A: // sub rx, ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                int32_t temp_rx = vm.registers[rx];
                int32_t temp = vm.registers[rx] - vm.registers[ry];
                fprintf(output, "0x%04X->SUB_R%d-=R%d=0x%08X-0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, vm.registers[ry], temp);
                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // sub dword ptr [rdi + rx], eax
                vm.executable_code[index++] = 0x29;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // inc dword ptr [rsi + 80]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x0B: // and rx, ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                int32_t temp_rx = vm.registers[rx];
                int32_t temp = vm.registers[rx] & vm.registers[ry];
                fprintf(output, "0x%04X->AND_R%d&=R%d=0x%08X&0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, vm.registers[ry], temp);

                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // and dword ptr [rdi + rx], eax
                vm.executable_code[index++] = 0x21;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // inc dword ptr [rsi + 88]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x0C: // or rx, ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                int32_t temp_rx = vm.registers[rx];
                int32_t temp = vm.registers[rx] | vm.registers[ry];

                fprintf(output, "0x%04X->OR_R%d|=R%d=0x%08X|0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, vm.registers[ry], temp);

                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // or dword ptr [rdi + rx], eax
                vm.executable_code[index++] = 0x09;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // inc dword ptr [rsi + 96]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x0D: // xor rx, ry
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t ry = vm.memory[pc + 1] & 0x0F;
                int32_t temp_rx = vm.registers[rx];
                int32_t temp = vm.registers[rx] ^ vm.registers[ry];

                fprintf(output, "0x%04X->XOR_R%d^=R%d=0x%08X^0x%08X=0x%08X\n",
                        pc, (int)rx, (int)ry, temp_rx, vm.registers[ry], temp);

                rx = rx * 4;
                ry = ry * 4;
                // mov eax, dword ptr [rdi + ry]
                vm.executable_code[index++] = 0x8B;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = ry;
                // xor dword ptr [rdi + rx], eax
                vm.executable_code[index++] = 0x31;
                vm.executable_code[index++] = 0x47;
                vm.executable_code[index++] = rx;
                // inc dword ptr [rsi + 104]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x0E: // sal rx, i5
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t shift_left = vm.memory[pc + 3] & 0x1F;
                int32_t temp_rx = vm.registers[rx];
                int32_t temp = vm.registers[rx] << shift_left;

                fprintf(output, "0x%04X->SAL_R%d<<=%d=0x%08X<<%d=0x%08X\n",
                        pc, (int)rx, (int)shift_left, temp_rx, (int)shift_left, temp);

                rx = rx * 4;
                // shl dword ptr [rdi + rx], shift_left
                vm.executable_code[index++] = 0xC1;
                vm.executable_code[index++] = 0x67;
                vm.executable_code[index++] = rx;
                vm.executable_code[index++] = shift_left;
                // inc dword ptr [rsi + 112]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index++] = 0x90;
                vm.executable_code[index] = 0x90;
                break;
            }

            case 0x0F: // sar rx, i5
            {
                uint8_t rx = vm.memory[pc + 1] >> 4;
                uint8_t shift_right = vm.memory[pc + 3] & 0x1F;
                int32_t signed_val = vm.registers[rx];
                int32_t temp_rx = vm.registers[rx];
                signed_val >>= shift_right;

                fprintf(output, "0x%04X->SAR_R%d>>=%d=0x%08X>>%d=0x%08X\n",
                        pc, (int)rx, (int)shift_right, temp_rx, (int)shift_right, signed_val);

                rx = rx * 4;
                // sar dword ptr [rdi + rx], shift_right
                vm.executable_code[index++] = 0xC1;
                vm.executable_code[index++] = 0x7F;
                vm.executable_code[index++] = rx;
                vm.executable_code[index++] = shift_right;
                // inc dword ptr [rsi + 120]
                vm.executable_code[index++] = 0xFF;
                vm.executable_code[index++] = 0x06;
                vm.executable_code[index++] = 0x90;
                vm.executable_code[index] = 0x90;
                break;
            }

            default:
            {
                break;
            }
            }
        }
        uint8_t *jit_addr = vm.executable_code + (pc * 4);
        JitFunc func = (JitFunc)jit_addr;
        uintptr_t result = func(&vm.registers[0], &vm.instruction_counts[pc / 4], &vm.memory[0], &vm.save_bool);
        if (result >= vm.code_base && result < vm.code_base + SIZE_CODE)
        {
            temp_pc = (uint32_t)(result - vm.code_base) / 4;
            temp_pc--;
        }
        else
        {
            temp_pc = (uint32_t)result;
            if (temp_pc >= MEMORY_SIZE)
            {
                break;
            }
        }
        pc = temp_pc;
    }

    fclose(output);
    output = fopen(argv[2], "w");
    if (!output)
    {
        fprintf(stderr, "Erro: nao foi possivel recriar %s\n", argv[2]);
        munmap(vm.executable_code, page_size);
        return 1;
    }

    print_formatted_output(output, &vm, pos, temp_pc);

    fclose(output);

    munmap(vm.executable_code, page_size);

    return 0;
}