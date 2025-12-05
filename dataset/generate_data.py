import numpy as np
import os

def generate_data(N=1000000, K=16, output_dir="."):
    np.random.seed(42) 
    
    print(f"Gerando N={N} pontos e K={K} centroids...")
    
    initial_centroids = np.linspace(5, K*10 - 5, K) + np.random.rand(K) * 2 - 1
    
    centroids_path = os.path.join(output_dir, "centroides_iniciais.csv")
    np.savetxt(centroids_path, initial_centroids, fmt='%.6f')
    print(f"Salvando centroids em {centroids_path}")

    points_per_cluster = N // K
    all_points = []
    
    for i in range(K):
        cluster_points = np.random.normal(loc=initial_centroids[i], scale=2.0, size=points_per_cluster)
        all_points.append(cluster_points)

    remaining = N - (points_per_cluster * K)
    if remaining > 0:
        all_points.append(np.random.normal(loc=initial_centroids[0], scale=2.0, size=remaining))

    final_data = np.concatenate(all_points)
    np.random.shuffle(final_data)
    
    data_path = os.path.join(output_dir, "dados.csv")
    np.savetxt(data_path, final_data, fmt='%.6f')
    print(f"Salvando dados em {data_path}")

if __name__ == "__main__":
    generate_data(output_dir=os.path.dirname(os.path.abspath(__file__)))
