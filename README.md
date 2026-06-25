````md
<div align="center">

# 🧠 Interface Hardware e Software

### Estudos práticos sobre a ponte entre **programas**, **sistema operacional**, **compiladores** e **hardware**

<br>

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Assembly](https://img.shields.io/badge/Assembly%20AMD64-6E4C13?style=for-the-badge&logo=assemblyscript&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)
![GCC](https://img.shields.io/badge/GCC-333333?style=for-the-badge&logo=gnu&logoColor=white)
![LLVM](https://img.shields.io/badge/LLVM-262D3A?style=for-the-badge&logo=llvm&logoColor=white)
![Makefile](https://img.shields.io/badge/Makefile-427819?style=for-the-badge&logo=gnu&logoColor=white)

<br>

[![Status](https://img.shields.io/badge/status-em%20desenvolvimento-blue?style=flat-square)](#)
[![Disciplina](https://img.shields.io/badge/disciplina-Interface%20Hardware--Software-purple?style=flat-square)](#)
[![Plataforma](https://img.shields.io/badge/plataforma-GNU%2FLinux-orange?style=flat-square)](#)
[![Arquitetura](https://img.shields.io/badge/arquitetura-x86--64%20%2F%20AMD64-red?style=flat-square)](#)

<br>

> Repositório acadêmico com implementações, experimentos e projetos voltados ao estudo de programação de baixo nível, Assembly AMD64, binários ELF, chamadas de sistema, compilação, profiling e interação entre software e hardware.

</div>

---

## 📌 Visão geral

Este repositório reúne atividades da disciplina **Interface Hardware e Software**, com foco em entender o que acontece entre escrever um programa e vê-lo executar no processador.

A proposta é estudar, de forma prática, o caminho:

```mermaid
flowchart LR
    A[Código-fonte em C] --> B[Compilador]
    B --> C[Assembly]
    C --> D[Arquivo objeto]
    D --> E[Executável ELF]
    E --> F[Sistema Operacional]
    F --> G[Hardware / CPU]
````

Ao longo do repositório, são explorados exemplos em **C**, **Assembly AMD64**, ferramentas GNU/Linux, análise de binários, geração de código intermediário, profiling e módulos de kernel.

---

## 🎯 Objetivos de aprendizagem

Ao estudar este repositório, o objetivo é desenvolver uma visão mais profunda sobre:

| Área                  | O que é estudado                                                         |
| --------------------- | ------------------------------------------------------------------------ |
| 🧩 **Compilação**     | Como C é transformado em Assembly, objeto e executável.                  |
| ⚙️ **Assembly AMD64** | Registradores, instruções, syscalls, pilha e fluxo de execução.          |
| 🧠 **Arquitetura**    | Relação entre CPU, memória, instruções e sistema operacional.            |
| 📦 **Binários ELF**   | Estrutura de executáveis Linux e análise com ferramentas de baixo nível. |
| 🚀 **Desempenho**     | Profiling, medição de tempo, eventos de CPU e otimização.                |
| 🐧 **Linux e Kernel** | Chamadas de sistema, módulos de kernel e execução privilegiada.          |

---

## 🗂️ Estrutura do repositório

| Diretório                          | Descrição                                                                       |
| ---------------------------------- | ------------------------------------------------------------------------------- |
| `Assembly (AMD64)/`                | Exemplos em Assembly AMD64 para praticar instruções, registradores e syscalls.  |
| `Aulas/`                           | Materiais de apoio da disciplina.                                               |
| `Avaliação de Desempenho (Perf)/`  | Experimentos de medição de desempenho com `perf`.                               |
| `Avaliação de Desempenho (gprof)/` | Experimentos de profiling com `gprof`.                                          |
| `Implementação 1/`                 | Atividade prática envolvendo C, Assembly, objeto e executável.                  |
| `Implementação 2/`                 | Atividade prática com foco em análise, compilação e execução.                   |
| `Implementação 3/`                 | Atividade prática complementar da disciplina.                                   |
| `JIT/`                             | Experimentos com geração/otimização de código e exponenciação modular.          |
| `MOD/`                             | Exemplos de módulos de kernel Linux e uso de `Makefile`.                        |
| `Projeto 1/`                       | Projeto em Assembly relacionado à conversão/decodificação de imagem para ASCII. |
| `Projeto 2/`                       | Projeto em C voltado a busca exaustiva e análise de desempenho.                 |
| `Projeto 3/PicoQuickProcessor/`    | Implementações e versões otimizadas do PicoQuickProcessor.                      |
| `VP1/`                             | Experimento com C, Assembly, linker script e execução controlada.               |
| `VP2/`                             | Continuação de experimentos com C, Assembly, syscalls e linker script.          |

---

## 🚀 Como clonar

```bash
git clone https://github.com/thejosephantony/InterfaceHardwareSoftware.git
cd InterfaceHardwareSoftware
```

---

## ▶️ Como compilar e executar

### Compilar um programa em C

```bash
gcc -Wall -Wextra -g arquivo.c -o programa
./programa
```

### Gerar Assembly a partir de C

```bash
gcc -S -masm=intel arquivo.c -o arquivo.s
```

### Gerar LLVM IR com Clang

```bash
clang -S -emit-llvm arquivo.c -o arquivo.ll
```

### Montar e executar Assembly com NASM

```bash
nasm -f elf64 arquivo.asm -o arquivo.o
ld arquivo.o -o programa
./programa
```

### Inspecionar um executável

```bash
objdump -d -M intel programa
readelf -h programa
readelf -S programa
readelf -s programa
```

---

## 🧪 Projetos principais

### 🖼️ Projeto 1 — Image to ASCII em Assembly

Projeto voltado à manipulação de dados em baixo nível e produção de saída textual a partir de uma entrada estruturada.

### 🔐 Projeto 2 — Busca exaustiva e desempenho

Projeto em C com foco em busca exaustiva, análise de custo computacional e avaliação de desempenho.

### ⚡ Projeto 3 — PicoQuickProcessor

Projeto com múltiplas versões do **PicoQuickProcessor**, incluindo implementações base, versões otimizadas e experimentos relacionados a desempenho.

### 🐧 MOD — Módulos de kernel Linux

Diretório com exemplos de módulos de kernel, incluindo `hello_world`, parâmetros de módulo, threads de kernel e produtor/consumidor.

---

## 🧰 Ferramentas utilizadas

| Ferramenta | Finalidade                                                   |
| ---------- | ------------------------------------------------------------ |
| `gcc`      | Compilar programas em C e Assembly.                          |
| `clang`    | Compilar C e gerar LLVM IR.                                  |
| `nasm`     | Montar programas Assembly com sintaxe Intel/NASM.            |
| `ld`       | Linkar arquivos objeto e gerar executáveis.                  |
| `make`     | Automatizar processos de compilação.                         |
| `objdump`  | Desmontar e inspecionar executáveis.                         |
| `readelf`  | Inspecionar cabeçalhos, seções e símbolos ELF.               |
| `gdb`      | Depurar programas e observar registradores, memória e fluxo. |
| `gprof`    | Gerar relatórios de profiling por instrumentação.            |
| `perf`     | Medir eventos de desempenho em nível de sistema/processador. |
| `time`     | Medir tempo de execução.                                     |
| `strace`   | Observar chamadas de sistema realizadas por um programa.     |

---

## 🔮 Melhorias futuras

* [ ] Criar um `README.md` específico em cada projeto.
* [ ] Padronizar nomes de arquivos e diretórios.
* [ ] Adicionar `Makefile` em todas as atividades executáveis.
* [ ] Criar scripts de execução e teste.
* [ ] Separar arquivos-fonte de arquivos gerados.
* [ ] Adicionar um `.gitignore` mais completo.
* [ ] Documentar entradas, saídas e comandos de cada atividade.
* [ ] Adicionar diagramas sobre compilação, linking e execução.
* [ ] Criar relatórios comparativos de desempenho.
* [ ] Configurar GitHub Actions para validar builds em C.

---

## ⚠️ Aviso acadêmico

Este repositório possui finalidade **acadêmica e educacional**.

Os conteúdos relacionados a engenharia reversa, busca exaustiva, análise de binários, profiling, JIT e módulos de kernel devem ser usados apenas em ambiente próprio, controlado e autorizado.

Não utilize os códigos, ideias ou técnicas deste repositório para acesso indevido, violação de sistemas, quebra de credenciais reais, exploração de terceiros ou qualquer atividade não autorizada.

---

## 👨‍💻 Autor

Desenvolvido por **Joseph Antony** como parte dos estudos da disciplina **Interface Hardware e Software**.

<div align="center">

<br>

**Interface Hardware e Software**
*C, Assembly, Linux, Compiladores, Binários ELF e Baixo Nível*

<br>

⬆️ [Voltar ao topo](#-interface-hardware-e-software)

</div>
```
