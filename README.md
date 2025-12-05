# Projeto PCD – K-Means 1D com Paralelização Progressiva

Este repositório contém a implementação e estudo do algoritmo **K-Means 1D**, explorando diferentes estratégias de paralelização para avaliar ganhos de desempenho em relação à versão serial.

O objetivo é comparar a eficiência computacional entre as seguintes abordagens:
- **Serial**: Implementação base (CPU, 1 thread).
- **OpenMP**: Paralelismo em memória compartilhada (Multithreading CPU).
- **MPI**: Paralelismo em memória distribuída (Cluster/Múltiplos processos).
- **CUDA**: Paralelismo massivo em GPU (NVIDIA).

---

## 📂 Estrutura do Projeto

O projeto está organizado em diretórios conforme a tecnologia utilizada:

- [**/dataset**](./dataset/): Scripts para geração e o próprio dataset padronizado utilizado em todos os testes.
- [**/serial**](./serial/): Código fonte e notebooks para a versão sequencial (baseline).
- [**/openmp**](./openmp/): Código fonte e notebooks para a versão paralela com OpenMP.
- [**/mpi**](./mpi/): Código fonte e notebooks para a versão distribuída com MPI.
- [**/cuda**](./cuda/): Código fonte e notebooks para a versão acelerada por GPU (CUDA).

---

## 📊 Dataset Padronizado

Para garantir a consistência dos testes de desempenho, **todos os experimentos devem utilizar o mesmo conjunto de dados**.

O script de geração encontra-se em `dataset/generate_data.py`.
Este script gera, por padrão:
- **1.000.000 (1 milhão)** de pontos.
- **16 Clusters** (K=16).
- Arquivos de saída: `dados.csv` (pontos) e `centroides_iniciais.csv`.

### Como gerar o dataset
Caso os arquivos `.csv` não estejam presentes na pasta `dataset` ou você queira regenerá-los, execute:

```bash
cd dataset
python generate_data.py
```

Isso garantirá que as execuções Serial, OpenMP, MPI e CUDA partam exatamente da mesma condição inicial.

---

## 🚀 Como Executar

Cada diretório possui seu próprio **README.md** e/ou **Notebook Jupyter (`.ipynb`)** com instruções específicas de compilação e execução, pois as dependências variam (GCC, MS-MPI, NVCC).

**Passo a passo geral:**
1. **Gere o dataset** (se ainda não existir) usando o passo acima.
2. **Navegue até a estratégia desejada** (ex: `cd openmp`).
3. **Leia o README local** daquela pasta para instruções de pré-requisitos e comandos.
4. **Execute os Notebooks**: A forma recomendada de execução e análise é através dos Jupyter Notebooks disponíveis em cada subpasta, que orquestram a compilação, execução e geração de gráficos.

### Requisitos Comuns
- **Python 3.8+**
- **Jupyter Notebook** ou **VS Code** (com extensão Jupyter)
- **Compilador C/C++** (GCC/MinGW para Windows, g++ para Linux)

---
*Este projeto foi desenvolvido como parte da disciplina de Programação Concorrente e Distribuída (PCD).*
