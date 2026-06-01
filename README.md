# Interface Hardware e Software (IHS)

![Assembly](https://img.shields.io/badge/Assembly-AMD64-blue?style=for-the-badge)
![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-Ubuntu-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![GCC](https://img.shields.io/badge/GCC-Compilador-663399?style=for-the-badge&logo=gnu&logoColor=white)
![LLVM](https://img.shields.io/badge/LLVM-IR-262D3A?style=for-the-badge&logo=llvm&logoColor=white)

Repositório acadêmico da disciplina **Interface Hardware e Software**, voltado ao estudo da relação entre programas, sistema operacional, compiladores e hardware. O objetivo é compreender como um código escrito em linguagem de alto nível é traduzido, carregado, executado e analisado em uma arquitetura real.

O foco principal está em **programação de baixo nível**, **Assembly AMD64**, **binários ELF**, **chamadas de sistema**, **registradores**, **memória**, **compilação** e **análise de desempenho**.

---

## 🎯 Objetivo da disciplina

A disciplina de Interface Hardware e Software busca aproximar dois níveis fundamentais da computação:

- o **software**, representado por programas, linguagens, compiladores e sistemas operacionais;
- o **hardware**, representado por processadores, registradores, memória, instruções de máquina e dispositivos.

Neste repositório, os estudos são organizados em exemplos práticos que ajudam a entender o caminho entre o código-fonte e a execução no processador.

---

## 🧠 Conteúdos estudados

### Arquitetura e baixo nível

- Arquitetura **x86-64 / AMD64**;
- registradores de uso geral, como `rax`, `rbx`, `rcx`, `rdx`, `rdi`, `rsi`, entre outros;
- instruções básicas de movimentação e aritmética, como `mov`, `add`, `sub`, `imul`, `div` e `xor`;
- pilha de execução, chamadas de função e convenções de chamada;
- organização da memória em seções como `.text`, `.data` e `.bss`.

### Assembly e sistema operacional

- escrita de programas simples em Assembly;
- uso de **syscalls** no Linux;
- finalização de programas com a syscall `exit`;
- manipulação direta de registradores;
- diferença entre código Assembly, código de máquina e executável.

### Compilação e representação intermediária

- compilação de programas em C com `gcc` e `clang`;
- geração e análise de código Assembly;
- geração de **LLVM IR**;
- análise do processo de tradução entre código-fonte, representação intermediária, Assembly e binário.

### Análise de binários

- estrutura de arquivos executáveis **ELF**;
- uso de `objdump` para inspecionar executáveis;
- leitura de trechos de disassembly;
- comparação entre código C, Assembly gerado e instruções executadas.

### Profiling e desempenho

- comparação entre implementações iterativas e recursivas;
- uso de `gprof` para análise de chamadas de função;
- uso de `perf` para análise de desempenho em nível mais próximo do hardware;
- observação de tempo de execução, chamadas, instruções e possíveis gargalos.

---

## 📂 O que este repositório contém

Este repositório reúne atividades e experimentos relacionados a:

| Tipo de conteúdo | Descrição |
| :--- | :--- |
| Exemplos em Assembly AMD64 | Programas simples para praticar instruções, registradores e syscalls. |
| Programas em C | Códigos usados para observar compilação, otimização e desempenho. |
| Saídas de ferramentas | Resultados de `objdump`, `gprof`, `perf` e outras ferramentas de análise. |
| LLVM IR | Representações intermediárias geradas a partir de programas em C. |
| Estudos de desempenho | Comparações entre diferentes formas de implementação, como funções iterativas e recursivas. |

---

## 🛠️ Ferramentas utilizadas

| Ferramenta | Finalidade |
| :--- | :--- |
| `gcc` | Compilar programas em C e Assembly no ambiente GNU/Linux. |
| `clang` | Compilar C e gerar LLVM IR. |
| `nasm` | Montar programas Assembly com sintaxe Intel/NASM. |
| `ld` | Linkar arquivos objeto gerados pelo assembler. |
| `objdump` | Inspecionar e desmontar executáveis. |
| `gdb` | Depurar programas e observar registradores/memória. |
| `gprof` | Gerar relatórios de profiling baseados em instrumentação. |
| `perf` | Medir desempenho usando eventos do sistema e do processador. |
| `strace` | Observar chamadas de sistema realizadas por um programa. |

---

## 💻 Ambiente de desenvolvimento

Os códigos foram desenvolvidos e testados em ambiente **Linux Ubuntu**, incluindo uso via **WSL no Windows**.

Ambiente recomendado:

- Ubuntu ou outra distribuição Linux;
- terminal Bash;
- `gcc`, `clang`, `nasm`, `binutils`, `gdb`, `perf` e `gprof` instalados.

Para instalar parte das ferramentas no Ubuntu:

```bash
sudo apt update
sudo apt install build-essential nasm clang llvm binutils gdb linux-tools-common linux-tools-generic
```

> Observação: em ambientes WSL, algumas funções do `perf` podem ter limitações dependendo da configuração do sistema.

---

## 🚀 Como clonar o repositório

```bash
git clone https://github.com/thejosephantony/InterfaceHardwareSoftware.git
cd InterfaceHardwareSoftware
```

---

## ▶️ Como executar exemplos

### 1. Compilando um programa em C

```bash
gcc -Wall -g arquivo.c -o programa
./programa
```

Com profiling usando `gprof`:

```bash
gcc -Wall -g -pg arquivo.c -o programa
./programa
gprof ./programa gmon.out > resultado.txt
```

### 2. Gerando LLVM IR com Clang

```bash
clang -S -emit-llvm arquivo.c -o arquivo.ll
```

### 3. Inspecionando um executável com objdump

```bash
objdump -d -M intel programa
```

Para desmontar todas as seções do binário:

```bash
objdump -D -z -M intel programa
```

### 4. Montando e executando Assembly com NASM

```bash
nasm -f elf64 arquivo.asm -o arquivo.o
ld arquivo.o -o programa
./programa
```

Para visualizar o código de saída do programa:

```bash
echo $?
```

### 5. Compilando Assembly no formato GNU AS

Caso o arquivo use sintaxe compatível com o `gcc`/GNU assembler:

```bash
gcc -no-pie arquivo.s -o programa
./programa
```

---

## 🔎 Exemplo conceitual: syscall `exit`

Um programa Assembly mínimo em Linux pode encerrar sua execução usando a syscall `exit`:

```asm
section .text

global _start

_start:
    mov rax, 60    ; número da syscall exit no Linux x86-64
    mov rdi, 0     ; código de saída
    syscall        ; chamada ao kernel
```

Nesse exemplo:

- `rax` recebe o número da chamada de sistema;
- `rdi` recebe o primeiro argumento da syscall;
- `syscall` transfere o controle para o kernel.

---

## 📊 Exemplos de análise de desempenho

Para comparar diferentes implementações, como fatorial iterativo e fatorial recursivo, podem ser usadas ferramentas como `gprof` e `perf`.

Exemplo com `perf`:

```bash
perf stat ./programa
```

Exemplo com `time`:

```bash
time ./programa
```

Essas ferramentas ajudam a observar diferenças entre:

- tempo real de execução;
- tempo gasto em modo usuário;
- tempo gasto em chamadas de sistema;
- quantidade de chamadas de função;
- impacto de otimizações do compilador.

---

## 📌 Conceitos importantes

Durante os estudos, alguns conceitos aparecem com frequência:

- **Assembly**: representação textual de instruções de máquina;
- **código de máquina**: instruções binárias executadas pelo processador;
- **ELF**: formato comum de executáveis em sistemas Linux;
- **ABI**: conjunto de regras para chamadas de função, passagem de parâmetros e uso da pilha;
- **syscall**: mecanismo usado por programas para solicitar serviços ao kernel;
- **profiling**: análise do comportamento de execução de um programa;
- **LLVM IR**: representação intermediária usada pelo ecossistema LLVM.

---

## 📚 Aprendizados principais

Este repositório reforça a ideia de que entender a interface entre hardware e software permite:

- escrever programas mais eficientes;
- compreender melhor o funcionamento de compiladores;
- interpretar mensagens e saídas de ferramentas de baixo nível;
- analisar binários e instruções geradas;
- entender a relação entre código, memória, processador e sistema operacional;
- desenvolver uma base mais sólida para sistemas operacionais, arquitetura de computadores, compiladores e segurança.

---

## 🧩 Possíveis melhorias futuras

- Adicionar um `Makefile` para automatizar a compilação dos exemplos;
- separar os códigos por temas ou aulas;
- incluir comentários adicionais nos arquivos Assembly;
- documentar comandos usados em cada atividade;
- adicionar exemplos com entrada e saída esperadas;
- incluir relatórios de profiling mais organizados.

---

## 👨‍💻 Autor

Desenvolvido por **Joseph Antony** como parte dos estudos da disciplina **Interface Hardware e Software**.
