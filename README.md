# Interface Hardware e Software (IHS)

![C](https://img.shields.io/badge/C-00599C?style=for-the-badge\&logo=c\&logoColor=white)
![Assembly](https://img.shields.io/badge/Assembly-6E4C13?style=for-the-badge\&logo=assemblyscript\&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge\&logo=linux\&logoColor=black)
![GCC](https://img.shields.io/badge/GCC-333333?style=for-the-badge\&logo=gnu\&logoColor=white)
![LLVM](https://img.shields.io/badge/LLVM-262D3A?style=for-the-badge\&logo=llvm\&logoColor=white)
![Makefile](https://img.shields.io/badge/Makefile-427819?style=for-the-badge\&logo=gnu\&logoColor=white)

Repositório acadêmico dedicado à disciplina **Interface Hardware e Software**, com implementações, experimentos e projetos voltados ao estudo da relação entre programas, compiladores, sistema operacional e hardware.

O objetivo central é compreender, na prática, como um programa escrito em linguagem de alto nível é compilado, transformado em código de máquina, carregado na memória, executado pelo processador e analisado por ferramentas de baixo nível.

## Sumário

* [Sobre o repositório](#sobre-o-repositório)
* [Objetivos de aprendizagem](#objetivos-de-aprendizagem)
* [Conteúdos abordados](#conteúdos-abordados)
* [Estrutura do repositório](#estrutura-do-repositório)
* [Ambiente recomendado](#ambiente-recomendado)
* [Instalação das ferramentas](#instalação-das-ferramentas)
* [Como clonar o repositório](#como-clonar-o-repositório)
* [Como compilar e executar](#como-compilar-e-executar)
* [Projetos principais](#projetos-principais)
* [Ferramentas utilizadas](#ferramentas-utilizadas)
* [Boas práticas adotadas](#boas-práticas-adotadas)
* [Possíveis melhorias futuras](#possíveis-melhorias-futuras)
* [Aviso de uso acadêmico](#aviso-de-uso-acadêmico)
* [Autor](#autor)

## Sobre o repositório

Este repositório reúne códigos, materiais e experimentos práticos sobre programação de baixo nível, arquitetura x86-64/AMD64, arquivos executáveis, chamadas de sistema, análise de desempenho, geração de código, módulos de kernel e execução em ambientes próximos ao hardware.

A proposta é estudar a interface entre hardware e software não apenas de forma teórica, mas também por meio de implementações em **C**, **Assembly**, **Makefile**, ferramentas GNU/Linux e utilitários de inspeção de binários.

## Objetivos de aprendizagem

Ao longo dos exemplos e projetos, o repositório busca desenvolver os seguintes conhecimentos:

* Entender a relação entre código-fonte, compilação, Assembly, código de máquina e executável.
* Compreender a organização de memória de programas em execução.
* Manipular registradores, instruções e chamadas de sistema em Assembly AMD64.
* Analisar arquivos executáveis no formato ELF.
* Observar o funcionamento de compiladores e representações intermediárias.
* Medir desempenho com ferramentas como `gprof`, `perf` e `time`.
* Investigar otimizações de código e diferenças entre versões de implementação.
* Explorar conceitos de módulos de kernel, geração de código e execução em baixo nível.
* Construir uma base sólida para sistemas operacionais, compiladores, arquitetura de computadores e engenharia reversa.

## Conteúdos abordados

### Arquitetura e programação de baixo nível

* Arquitetura x86-64 / AMD64.
* Registradores de uso geral, como `rax`, `rbx`, `rcx`, `rdx`, `rdi`, `rsi`, `rsp` e `rbp`.
* Instruções básicas de movimentação, aritmética e controle de fluxo.
* Pilha de execução, chamadas de função e convenções de chamada.
* Organização de memória em seções como `.text`, `.data`, `.bss`, heap e stack.

### Assembly e sistema operacional

* Escrita de programas em Assembly.
* Uso de syscalls no Linux.
* Manipulação direta de registradores.
* Entrada e saída em baixo nível.
* Diferenças entre Assembly, código de máquina, objeto e executável.

### Compilação e geração de código

* Compilação de programas em C com `gcc` e `clang`.
* Geração de Assembly a partir de código C.
* Geração de LLVM IR.
* Uso de linker scripts em experimentos específicos.
* Construção de executáveis com `ld`, `gcc` e `make`.

### Análise de binários

* Estrutura de executáveis ELF.
* Inspeção de binários com `objdump`, `readelf` e ferramentas relacionadas.
* Comparação entre código C, Assembly gerado e instruções executadas.
* Observação de símbolos, seções, chamadas e endereços.

### Avaliação de desempenho

* Profiling com `gprof`.
* Medição de desempenho com `perf`.
* Comparação entre implementações diferentes.
* Análise de tempo de execução, chamadas de função, eventos de CPU e impacto de otimizações.

### Sistemas e kernel

* Experimentos com módulos de kernel Linux.
* Uso de `Makefile` para compilação automatizada.
* Estudo de threads de kernel, parâmetros de módulo e sincronização.
* Relação entre aplicação, sistema operacional e hardware.

## Estrutura do repositório

| Diretório                          | Conteúdo principal                                                                                  |
| ---------------------------------- | --------------------------------------------------------------------------------------------------- |
| `Assembly (AMD64)/`                | Exemplos básicos em Assembly AMD64, incluindo operações aritméticas e estrutura mínima de programa. |
| `Aulas/`                           | Materiais de apoio da disciplina em PDF.                                                            |
| `Avaliação de Desempenho (Perf)/`  | Experimentos de medição com `perf`.                                                                 |
| `Avaliação de Desempenho (gprof)/` | Experimentos de profiling com `gprof`.                                                              |
| `Implementação 1/`                 | Atividade prática envolvendo C, Assembly, objeto e executável.                                      |
| `Implementação 2/`                 | Atividade prática com foco em análise, compilação e execução.                                       |
| `Implementação 3/`                 | Atividade prática complementar da disciplina.                                                       |
| `JIT/`                             | Experimentos envolvendo geração/otimização de código e exponenciação modular.                       |
| `MOD/`                             | Exemplos de módulos de kernel Linux e uso de `Makefile`.                                            |
| `Projeto 1/`                       | Projeto em Assembly relacionado à conversão/decodificação de imagem para ASCII.                     |
| `Projeto 2/`                       | Projeto em C voltado a busca exaustiva e avaliação de desempenho em contexto acadêmico.             |
| `Projeto 3/PicoQuickProcessor/`    | Implementações e versões otimizadas do PicoQuickProcessor.                                          |
| `VP1/`                             | Experimento com C, Assembly, linker script e execução em ambiente controlado.                       |
| `VP2/`                             | Continuação de experimentos com C, Assembly, syscalls, linker script e especificação de build.      |

## Ambiente recomendado

Os exemplos foram pensados para execução em ambiente GNU/Linux.

Ambiente sugerido:

* Ubuntu, Debian ou distribuição Linux equivalente.
* WSL2 no Windows, quando aplicável.
* Terminal Bash.
* Compiladores e ferramentas GNU.
* Permissão de administrador para experimentos que envolvam `perf` ou módulos de kernel.

> Observação: algumas ferramentas, como `perf` e módulos de kernel, podem apresentar limitações em WSL dependendo da configuração do ambiente.

## Instalação das ferramentas

No Ubuntu/Debian, parte das ferramentas pode ser instalada com:

```bash
sudo apt update
sudo apt install build-essential nasm clang llvm binutils gdb make linux-tools-common linux-tools-generic
```

Para módulos de kernel, também pode ser necessário instalar os headers do kernel:

```bash
sudo apt install linux-headers-$(uname -r)
```

Em WSL, o pacote de headers e o funcionamento de módulos de kernel podem variar conforme a versão e a configuração do sistema.

## Como clonar o repositório

```bash
git clone https://github.com/thejosephantony/InterfaceHardwareSoftware.git
cd InterfaceHardwareSoftware
```

## Como compilar e executar

Como o repositório contém atividades diferentes, os comandos podem variar conforme a pasta. Abaixo estão exemplos gerais.

### Compilar um programa em C

```bash
gcc -Wall -Wextra -g arquivo.c -o programa
./programa
```

Com otimização:

```bash
gcc -Wall -Wextra -O2 arquivo.c -o programa
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
nasm -f elf64 arquivo.s -o arquivo.o
ld arquivo.o -o programa
./programa
```

Para visualizar o código de saída do programa:

```bash
echo $?
```

### Compilar Assembly com GCC/GNU AS

Caso o arquivo Assembly esteja no formato aceito pelo GNU Assembler:

```bash
gcc -no-pie arquivo.s -o programa
./programa
```

### Inspecionar um executável

```bash
objdump -d -M intel programa
```

Para desmontar todas as seções:

```bash
objdump -D -z -M intel programa
```

Outros comandos úteis:

```bash
readelf -h programa
readelf -S programa
readelf -s programa
```

### Profiling com gprof

```bash
gcc -Wall -Wextra -pg arquivo.c -o programa
./programa
gprof ./programa gmon.out > relatorio-gprof.txt
```

### Medição com perf

```bash
perf stat ./programa
```

Também é possível medir eventos específicos:

```bash
perf stat -e cycles,instructions,cache-references,cache-misses ./programa
```

### Usar Makefile quando disponível

Algumas pastas possuem `Makefile`. Nesses casos, entre no diretório e execute:

```bash
make
```

Para limpar os arquivos gerados, quando houver alvo de limpeza:

```bash
make clean
```

## Projetos principais

### Projeto 1 — Image to ASCII em Assembly

Projeto voltado à manipulação de dados em baixo nível e produção de saída textual a partir de uma entrada estruturada. O objetivo é exercitar Assembly AMD64, leitura de dados, processamento e geração de saída.

Exemplo geral de execução:

```bash
cd "Projeto 1"
# Ajuste o comando de compilação conforme a sintaxe Assembly usada no arquivo.
gcc -no-pie arquivo.s -o image2ascii
./image2ascii < image2ascii.input.txt > saida.txt
```

### Projeto 2 — Busca exaustiva e desempenho

Projeto em C com foco em busca exaustiva, análise de tempo de execução e otimização. A proposta é estudar custo computacional, estratégias de implementação e impacto de decisões de baixo nível.

Exemplo geral:

```bash
cd "Projeto 2"
gcc -Wall -Wextra -O2 arquivo.c -o programa
./programa
```

### Projeto 3 — PicoQuickProcessor

Projeto com múltiplas versões do **PicoQuickProcessor**, incluindo implementações sem otimização, versões melhoradas e experimentos relacionados a JIT/otimização.

Exemplo geral:

```bash
cd "Projeto 3/PicoQuickProcessor"
gcc -Wall -Wextra -O2 PicoQuickProcessor_entrega_final.c -o pqp
./pqp < pqp.input.txt > saida.txt
diff -u pqp.output.txt saida.txt
```

### MOD — Módulos de kernel Linux

Diretório com exemplos de módulos de kernel e experimentos como `hello_world`, parâmetros de módulo, threads de kernel e produtor/consumidor.

Exemplo geral:

```bash
cd MOD
make
sudo insmod nome_do_modulo.ko
dmesg | tail -n 30
sudo rmmod nome_do_modulo
```

> Use esses exemplos com cuidado. Módulos de kernel rodam em modo privilegiado e podem afetar a estabilidade do sistema.

## Ferramentas utilizadas

| Ferramenta | Finalidade                                                               |
| ---------- | ------------------------------------------------------------------------ |
| `gcc`      | Compilar programas em C e Assembly no ecossistema GNU.                   |
| `clang`    | Compilar C e gerar LLVM IR.                                              |
| `nasm`     | Montar programas Assembly com sintaxe Intel/NASM.                        |
| `ld`       | Linkar arquivos objeto e gerar executáveis.                              |
| `make`     | Automatizar processos de compilação.                                     |
| `objdump`  | Desmontar e inspecionar executáveis.                                     |
| `readelf`  | Inspecionar cabeçalhos, seções e símbolos ELF.                           |
| `gdb`      | Depurar programas e observar registradores, memória e fluxo de execução. |
| `gprof`    | Gerar relatórios de profiling por instrumentação.                        |
| `perf`     | Medir eventos de desempenho em nível de sistema/processador.             |
| `time`     | Medir tempo de execução de comandos.                                     |
| `strace`   | Observar chamadas de sistema realizadas por um programa.                 |

## Exemplo conceitual: syscall `exit`

Um programa Assembly mínimo em Linux x86-64 pode encerrar sua execução com a syscall `exit`:

```asm
section .text

global _start

_start:
    mov rax, 60     ; número da syscall exit no Linux x86-64
    mov rdi, 0      ; código de saída
    syscall         ; chamada ao kernel
```

Nesse exemplo:

* `rax` armazena o número da chamada de sistema.
* `rdi` armazena o primeiro argumento da syscall.
* `syscall` transfere o controle para o kernel.
* O programa retorna código de saída `0`.

## Boas práticas adotadas

* Separação dos experimentos por tema, projeto ou atividade.
* Uso de C e Assembly para observar diferentes níveis de abstração.
* Registro de entradas e saídas esperadas em projetos específicos.
* Uso de ferramentas reais de análise e profiling.
* Comparação entre versões simples, otimizadas e experimentais.

## Possíveis melhorias futuras

* Criar um `README.md` específico dentro de cada projeto.
* Padronizar nomes de arquivos e diretórios.
* Adicionar `Makefile` em todas as atividades executáveis.
* Separar arquivos-fonte de binários gerados (`.o`, `.elf`, `.exe`, executáveis locais).
* Adicionar um `.gitignore` mais completo para arquivos de build.
* Incluir scripts de execução e teste para os principais projetos.
* Documentar entradas, saídas e comandos de cada implementação.
* Adicionar diagramas explicando o caminho: C → Assembly → objeto → ELF → execução.
* Criar relatórios comparativos de desempenho com tabelas e gráficos.
* Adicionar GitHub Actions para validar a compilação dos códigos em C.

## Aviso de uso acadêmico

Este repositório tem finalidade exclusivamente acadêmica e educacional.

Os conteúdos relacionados a engenharia reversa, busca exaustiva, análise de binários, profiling, JIT e módulos de kernel devem ser usados apenas em ambiente próprio, controlado e autorizado.

Não utilize os códigos, ideias ou técnicas deste repositório para acesso indevido, violação de sistemas, quebra de credenciais reais, exploração de terceiros ou qualquer atividade não autorizada.

## Autor

Desenvolvido por **Joseph Antony** como parte dos estudos da disciplina **Interface Hardware e Software**.
