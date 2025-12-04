/*
  kmeans_1d_mpi.c
  K-Means 1D com Paralelização MPI (Memória Distribuída)
  
  Estratégia:
  1. Processo 0 (Root) lê os dados (X) e centróides (C).
  2. Root distribui X entre os P processos (MPI_Scatterv).
  3. Todos os processos mantêm uma cópia de C.
  4. Loop:
     - Assignment local: cada processo calcula SSE e somas parciais para seu pedaço de X.
     - Redução Global: MPI_Allreduce soma SSE, somas e contadores de todos os processos.
     - Update local: todos os processos atualizam C com os valores globais.
     - Verificação de convergência.
  5. (Opcional) Root recolhe assignments finais (MPI_Gatherv).
  
  Compilar: mpicc -O2 kmeans_1d_mpi.c -o kmeans_1d_mpi -lm
  Executar: mpirun -np 4 ./kmeans_1d_mpi dados.csv centroides.csv ...
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

/* ---------- Funções Auxiliares (Leitura CSV) ---------- */
/* Apenas o Rank 0 vai usar estas funções para ler do disco */

static int count_rows(const char *path){
    FILE *f = fopen(path, "r");
    if(!f){ fprintf(stderr,"Erro ao abrir %s\n", path); exit(1); }
    int rows=0; char line[8192];
    while(fgets(line,sizeof(line),f)){
        int only_ws=1;
        for(char *p=line; *p; p++){
            if(*p!=' ' && *p!='\t' && *p!='\n' && *p!='\r'){ only_ws=0; break; }
        }
        if(!only_ws) rows++;
    }
    fclose(f);
    return rows;
}

static double *read_csv_1col(const char *path, int *n_out){
    int R = count_rows(path);
    if(R<=0){ fprintf(stderr,"Arquivo vazio: %s\n", path); exit(1); }
    double *A = (double*)malloc((size_t)R * sizeof(double));
    if(!A){ fprintf(stderr,"Sem memoria para %d linhas\n", R); exit(1); }
    FILE *f = fopen(path, "r");
    if(!f){ fprintf(stderr,"Erro ao abrir %s\n", path); free(A); exit(1); }
    char line[8192]; int r=0;
    while(fgets(line,sizeof(line),f)){
        int only_ws=1;
        for(char *p=line; *p; p++){
            if(*p!=' ' && *p!='\t' && *p!='\n' && *p!='\r'){ only_ws=0; break; }
        }
        if(only_ws) continue;
        const char *delim = ",; \t";
        char *tok = strtok(line, delim);
        if(!tok){ fprintf(stderr,"Linha %d sem valor em %s\n", r+1, path); free(A); fclose(f); exit(1); }
        A[r] = atof(tok);
        r++;
        if(r>R) break;
    }
    fclose(f); *n_out = R; return A;
}

static void write_centroids_csv(const char *path, const double *C, int K){
    if(!path) return;
    FILE *f = fopen(path, "w");
    if(!f){ fprintf(stderr,"Erro ao abrir %s para escrita\n", path); return; }
    for(int c=0;c<K;c++) fprintf(f, "%.6f\n", C[c]);
    fclose(f);
}

/* ---------- K-Means Core ---------- */

int main(int argc, char **argv){
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(rank == 0 && argc < 3){
        printf("Uso: %s dados.csv centroides.csv [max_iter] [eps] [out_assign] [out_centroids]\n", argv[0]);
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    // Parâmetros (lidos pelo rank 0 e transmitidos)
    int N_global = 0;
    int K = 0;
    int max_iter = 50;
    double eps = 1e-4;
    
    // Arrays globais (apenas no rank 0 inicialmente)
    double *X_global = NULL;
    double *C = NULL; // Todos precisam de C

    // 1. Leitura e Broadcast de Parâmetros
    if(rank == 0){
        X_global = read_csv_1col(argv[1], &N_global);
        C = read_csv_1col(argv[2], &K);
        if(argc > 3) max_iter = atoi(argv[3]);
        if(argc > 4) eps = atof(argv[4]);
    }

    // Broadcast de N, K, max_iter, eps
    MPI_Bcast(&N_global, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&K, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&max_iter, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&eps, 1, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Aloca C em todos os processos (se não for rank 0)
    if(rank != 0){
        C = (double*)malloc(K * sizeof(double));
    }
    // Broadcast dos centróides iniciais
    MPI_Bcast(C, K, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // 2. Distribuição de Dados (Scatterv)
    // Calcula quantos pontos cada processo vai receber
    int *sendcounts = NULL;
    int *displs = NULL;
    int N_local = N_global / size;
    int remainder = N_global % size;

    if(rank == 0){
        sendcounts = (int*)malloc(size * sizeof(int));
        displs = (int*)malloc(size * sizeof(int));
        int offset = 0;
        for(int i=0; i<size; i++){
            sendcounts[i] = N_local + (i < remainder ? 1 : 0);
            displs[i] = offset;
            offset += sendcounts[i];
        }
    }

    // Ajusta N_local do próprio processo
    N_local = N_local + (rank < remainder ? 1 : 0);

    // Aloca buffer local
    double *X_local = (double*)malloc(N_local * sizeof(double));
    int *assign_local = (int*)malloc(N_local * sizeof(int));

    // Scatterv
    MPI_Scatterv(X_global, sendcounts, displs, MPI_DOUBLE, 
                 X_local, N_local, MPI_DOUBLE, 
                 0, MPI_COMM_WORLD);

    // Libera X_global no root para economizar memória (opcional, mas bom para N grande)
    // Mas se formos fazer gather no final, precisaremos dele ou realocá-lo.
    // Vamos manter por enquanto se couber na RAM.

    // Buffers auxiliares para redução
    double *sum_local = (double*)malloc(K * sizeof(double));
    int *cnt_local = (int*)malloc(K * sizeof(int));
    double *sum_global = (double*)malloc(K * sizeof(double));
    int *cnt_global = (int*)malloc(K * sizeof(int));
    double *sse_history = (double*)malloc(max_iter * sizeof(double));

    // Sincronização antes de começar o tempo
    MPI_Barrier(MPI_COMM_WORLD);
    double t0 = MPI_Wtime();

    // 3. Loop K-Means
    double prev_sse = 1e300;
    double sse_global = 0.0;
    int it = 0;
    
    // Variável para medir tempo de comunicação (Allreduce)
    double total_comm_time = 0.0;

    for(it=0; it<max_iter; it++){
        // --- Assignment Local ---
        double sse_local = 0.0;
        // Zera acumuladores locais
        for(int c=0; c<K; c++){ sum_local[c] = 0.0; cnt_local[c] = 0; }

        for(int i=0; i<N_local; i++){
            double val = X_local[i];
            int best_c = -1;
            double best_d = 1e300;
            for(int c=0; c<K; c++){
                double diff = val - C[c];
                double d = diff*diff;
                if(d < best_d){ best_d = d; best_c = c; }
            }
            assign_local[i] = best_c;
            sse_local += best_d;
            
            // Acumula para update
            sum_local[best_c] += val;
            cnt_local[best_c]++;
        }

        // --- Redução Global (SSE, Sum, Cnt) ---
        double t_comm_start = MPI_Wtime();
        
        // SSE
        MPI_Allreduce(&sse_local, &sse_global, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        
        // Sum e Cnt
        MPI_Allreduce(sum_local, sum_global, K, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);
        MPI_Allreduce(cnt_local, cnt_global, K, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
        
        double t_comm_end = MPI_Wtime();
        total_comm_time += (t_comm_end - t_comm_start);

        // --- Update Local (Todos calculam o mesmo C novo) ---
        for(int c=0; c<K; c++){
            if(cnt_global[c] > 0)
                C[c] = sum_global[c] / cnt_global[c];
            else
                C[c] = C[c]; // Mantém (ou estratégia de reset)
        }

        // Histórico e Convergência
        if(rank == 0){
            sse_history[it] = sse_global;
            // printf("SSE_ITER_MPI: %d %.6f\n", it, sse_global); // Debug
        }

        double rel = fabs(sse_global - prev_sse) / (prev_sse > 0.0 ? prev_sse : 1.0);
        if(rel < eps && it > 0){ 
            it++; 
            break; 
        }
        prev_sse = sse_global;
    }

    double t1 = MPI_Wtime();
    double ms_total = (t1 - t0) * 1000.0;
    double ms_comm = total_comm_time * 1000.0;

    // 4. Finalização e Saída
    if(rank == 0){
        printf("K-means 1D (MPI)\n");
        printf("N=%d K=%d max_iter=%d eps=%g Processes=%d\n", N_global, K, max_iter, eps, size);
        printf("Iterações: %d | SSE final: %.6f | Tempo Total: %.1f ms | Tempo Comm: %.1f ms\n", 
               it, sse_global, ms_total, ms_comm);
        
        // Saída para parsing
        printf("FINAL_DATA: Time=%.1f Time_Comm=%.1f SSE=%.6f Iters=%d N=%d K=%d Procs=%d\n", 
               ms_total, ms_comm, sse_global, it, N_global, K, size);

        for(int i=0; i<it; i++){
            printf("SSE_HISTORY: %d %.6f\n", i, sse_history[i]);
        }

        // Salvar centróides
        if(argc > 6) write_centroids_csv(argv[6], C, K);
    }

    // (Opcional) Gather assignments se solicitado
    // if(argc > 5 && argv[5] != NULL) ...

    // Limpeza
    free(X_local); free(assign_local);
    free(sum_local); free(cnt_local);
    free(sum_global); free(cnt_global);
    free(C); free(sse_history);
    
    if(rank == 0){
        free(X_global);
        free(sendcounts);
        free(displs);
    }

    MPI_Finalize();
    return 0;
}
