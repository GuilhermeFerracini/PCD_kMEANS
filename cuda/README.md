---

# Projeto PCD – K-Means 1D com Paralelização Progressiva (Etapa 2 – CUDA)

Este README descreve a **Etapa 2 – Implementação CUDA** do projeto de K-Means 1D com paralelização progressiva.
O guia foi pensado especificamente para execução no **Google Colab**, garantindo que a GPU esteja habilitada corretamente.

---

## 🚀 1. Configuração do Ambiente no Google Colab

Para rodar código CUDA no Colab, **é obrigatório ativar uma GPU**.
Sem GPU, o Colab não conseguirá compilar ou executar código `.cu`.

### 🔧 Passo a Passo — Habilitando a GPU

1. Abra o notebook no Google Colab (ex: `Kmeans_Paralelo_CUDA.ipynb`).
2. No menu superior, clique em **Ambiente de execução**.
3. Clique em **Alterar tipo de ambiente de execução**.
4. No campo "Acelerador de hardware", selecione:
   **GPU (T4 recomendada)**
   (P100 ou V100 também funcionam.)
5. Clique em **Salvar**.
   O ambiente será reiniciado com suporte CUDA.

