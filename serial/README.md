# Projeto PCD – K-Means 1D com Paralelização Progressiva - Serial

## 🚀 Execução em Ambiente Windows (Local)
Para rodar este projeto em sua máquina local (Windows):

1. **Pré-requisitos**:
   - Compilador GCC instalado (recomendado MinGW-w64).
   - Python 3.x e Jupyter Notebook (ou VS Code com extensão Jupyter).

2. **Compilação e Execução**:
   - Abra o notebook `Serial_kmeans.ipynb` no VS Code ou Jupyter Lab.
   - O notebook contém comandos para compilar e executar o código C.
   - O comando de compilação típico (via célula de código) será:
     ```python
     !gcc -o kmeans_serial kmeans_1d_naive.c
     ```
   - Em seguida, o código é executado chamando o binário gerado (ex: `!kmeans_serial.exe`).

> **Nota:** Certifique-se de que o `gcc` está no PATH do seu sistema.

## 📈 Saídas
- Métricas de tempo e SSE por iteração
- Arquivos de saída: `assign.csv`, `centroids.csv`
- Gráficos:
  - Convergência do SSE
  - Speedup vs. Threads
  - Comparação de centróides
