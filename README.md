# Interface Hardware e Software (IHS)

![Assembly](https://img.shields.io/badge/Assembly-AMD64-blue?style=for-the-badge)
![C](https://img.shields.io/badge/C-00599C?style=for-the-badge&logo=c&logoColor=white)
![Linux](https://img.shields.io/badge/Linux-FCC624?style=for-the-badge&logo=linux&logoColor=black)

Repositório académico dedicado ao estudo da camada que liga o software aos recursos físicos do computador. O foco central é compreender como o código de alto nível é traduzido, carregado e executado pelo hardware, analisando desde instruções em linguagem de montagem até ao desempenho em tempo de execução.

---

## 🎯 Visão Geral

Este projeto documenta a exploração técnica do "abismo" entre linguagens de programação e o silício. Através do estudo de **Interface Hardware e Software**, abordamos temas fundamentais para o desenvolvimento de sistemas de baixo nível, segurança e análise de binários.

### Áreas de Aplicação:
- **Engenharia Reversa:** Análise de binários e lógica de programas sem código-fonte.
- **Otimização de Software:** Compreensão de como o compilador gera código e como a CPU o executa.
- **Sistemas de Baixo Nível:** Interação direta com o Kernel e gestão de memória.

---

## 📂 Estrutura do Repositório

O repositório está organizado para refletir o fluxo de desenvolvimento e análise:

* **`Assembly (AMD64)`**: Implementações puras em linguagem de montagem, focando no uso de registadores e syscalls.
* **`Implementação 1 & 2`**: Diferentes abordagens de lógica para comparação de algoritmos e técnicas de codificação.
* **`Avaliação de Desempenho (Perf & gprof)`**: Testes e relatórios de profiling para identificar gargalos de execução e otimizar o uso da CPU.

---

## 🧠 Conteúdos Estudados

### 🏗️ Arquitetura e Organização
* **Ciclo de Instrução:** Busca, decodificação e execução.
* **Hierarquia de Memória:** Gestão de Registadores, Cache (L1/L2/L3), RAM e Stack/Heap.
* **AMD64:** Estudo da arquitetura x86-64 e registadores de uso geral.

### 🛠️ Programação de Baixo Nível
* **Assembly (NASM):** Sintaxe Intel, manipulação de pilhas e convenções de chamada (Calling Conventions).
* **Syscalls:** Interação direta com o Kernel do Linux para operações de I/O.
* **Binários ELF:** Estrutura de executáveis e secções (`.text`, `.data`, `.bss`).

### ⚡ Análise de Desempenho
* **Profiling:** Uso de `gprof` e `perf` para análise de ciclos de CPU, cache misses e tempo de execução.
* **Otimização:** Análise de como mudanças no código influenciam a tradução para linguagem de máquina.

---

## 🔍 Ferramentas de Inspeção e Análise

Para a análise dos projetos, utilizamos o seguinte conjunto de ferramentas profissionais:

| Ferramenta | Utilidade |
| :--- | :--- |
| `nasm` | Assembler para arquitetura x86-64. |
| `gdb` | Depuração e inspeção de registadores/memória. |
| `objdump` | Desmontagem (disassembly) de binários para análise. |
| `perf` / `gprof` | Ferramentas de profiling e monitorização de performance. |
| `strace` | Monitorização de chamadas de sistema (syscalls). |

---

## 🚀 Como Explorar

```bash
# Clonar o repositório
git clone [https://github.com/thejosephantony/InterfaceHardwareSoftware.git](https://github.com/thejosephantony/InterfaceHardwareSoftware.git)

# Aceder ao diretório
cd InterfaceHardwareSoftware
