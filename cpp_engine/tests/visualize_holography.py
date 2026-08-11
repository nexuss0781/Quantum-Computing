import sys
import os
import math
import matplotlib.pyplot as plt
import networkx as nx

# Add the build directory to sys.path to find the compiled module
sys.path.append(os.path.join(os.path.dirname(__file__), '../build'))

import qau_cpp

def visualize_emergent_space():
    print("Synthesizing Emergent Space from Quantum Entanglement...")
    qvs = qau_cpp.QVS()
    
    # Create a system of 4 qubits
    # We will forge a specific entanglement structure: a chain 0-1-2-3
    # Qubit 0 and 1 entangled, 1 and 2 entangled, 2 and 3 entangled.
    # To do this in a single ASC, we manually set amplitudes for a GHZ-like or Cluster state.
    
    # For simplicity, let's create a Bell state between two qubits and see the distance
    # Then create a 3-qubit W-state.
    
    # 1. Bell State Case
    print("\n--- Scenario 1: Bell State (Qubit 0 & 1) ---")
    asc_id = qvs.create_asc(2)
    qvs.SUPERPOSE(asc_id, [[0, 0], [1, 1]]) # (|00> + |11>)/sqrt(2)
    asc = qvs.get_asc(asc_id)
    
    mi = qau_cpp.HolographyEngine.mutual_information(asc, [0], [1])
    dist = qau_cpp.HolographyEngine.emergent_distance(asc, 0, 1)
    print(f"Mutual Information I(0:1): {mi:.4f}")
    print(f"Emergent Distance d(0:1): {dist:.4f}")

    # 2. W-State Case (3 Qubits)
    print("\n--- Scenario 2: W-State (Qubit 0, 1, 2) ---")
    asc_id_w = qvs.create_asc(3)
    # W-state: (|100> + |010> + |001>) / sqrt(3)
    qvs.SUPERPOSE(asc_id_w, [[1, 0, 0], [0, 1, 0], [0, 0, 1]])
    asc_w = qvs.get_asc(asc_id_w)
    
    G = nx.Graph()
    for i in range(3):
        for j in range(i + 1, 3):
            mi = qau_cpp.HolographyEngine.mutual_information(asc_w, [i], [j])
            dist = qau_cpp.HolographyEngine.emergent_distance(asc_w, i, j)
            print(f"I({i}:{j}): {mi:.4f} | d({i}:{j}): {dist:.4f}")
            # Add edge with weight inversely proportional to distance for visualization
            G.add_edge(i, j, weight=1.0/max(0.1, dist))

    # Visualize the emergent graph
    plt.figure(figsize=(8, 6))
    pos = nx.spring_layout(G)
    nx.draw(G, pos, with_labels=True, node_color='skyblue', node_size=1500, font_size=15, width=2)
    plt.title("Emergent Holographic Space from W-State Entanglement")
    plt.savefig("emergent_space_w_state.png")
    print("\nVisualization saved as 'emergent_space_w_state.png'")

if __name__ == "__main__":
    visualize_emergent_space()
