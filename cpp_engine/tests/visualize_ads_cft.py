import sys
import os
import math
import matplotlib.pyplot as plt
import numpy as np

# Add the build directory to sys.path to find the compiled module
sys.path.append(os.path.join(os.path.dirname(__file__), '../build'))

import qau_cpp

def visualize_ads_bulk():
    print("Simulating AdS Bulk Discretization in the Poincaré Disk...")
    
    # Initialize a holographic network with 4 layers
    network = qau_cpp.HolographicNetwork(4, 6)
    
    bulk_nodes = network.bulk_nodes
    print(f"Generated {len(bulk_nodes)} bulk nodes in the AdS space.")
    
    plt.figure(figsize=(8, 8))
    
    # Draw the boundary circle
    theta = np.linspace(0, 2*np.pi, 100)
    plt.plot(np.cos(theta), np.sin(theta), 'k--', alpha=0.5, label="AdS Boundary")
    
    # Draw bulk nodes and connections
    for node in bulk_nodes:
        z = node.coord
        plt.scatter(z.real, z.imag, c='blue', s=20, alpha=0.6)
        
        for neighbor_id in node.neighbors:
            # Only draw each edge once
            if neighbor_id > node.id:
                neighbor = bulk_nodes[neighbor_id]
                zn = neighbor.coord
                plt.plot([z.real, zn.real], [z.imag, zn.imag], 'b-', alpha=0.2)

    plt.gca().set_aspect('equal')
    plt.title("Holographic Bulk Discretization (Poincaré Disk)")
    plt.legend()
    plt.savefig("ads_bulk_discretization.png")
    print("Visualization saved as 'ads_bulk_discretization.png'")

if __name__ == "__main__":
    visualize_ads_bulk()
