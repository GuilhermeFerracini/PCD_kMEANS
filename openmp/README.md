# Projeto PCD – K-Means 1D com Paralelização Progressiva - OpenMP

## 🚀 Execução no Google Colab
Para rodar este projeto no **Google Colab**:

1. Acesse o link do notebook:
   [Abrir no Google Colab](https://colab.research.google.com/drive/184bgv1JuWvcVZxaUuAiY-frUrrQcwOoN?usp=sharing)

2. Ou abra manualmente:
   - Vá para [Google Colab](https://colab.research.google.com/)
   - Clique em **Arquivo → Abrir notebook**
   - Vá na aba **GitHub**
   - Cole a URL do repositório deste projeto
   - Selecione o notebook `.ipynb` (ex: `ProjetoPCD-kmeans.ipynb`)

3. No Colab, execute as células **de cima para baixo**, garantindo:
   - Ambiente Linux padrão (CPU/GPU, conforme o experimento)
   - Permissão de execução de shell (códigos `%%shell`)

## 📈 Saídas
- Métricas de tempo e SSE por iteração
- Arquivos de saída: `assign.csv`, `centroids.csv`
- Gráficos:
  - Convergência do SSE
  - Speedup vs. Threads
  - Comparação de centróides

