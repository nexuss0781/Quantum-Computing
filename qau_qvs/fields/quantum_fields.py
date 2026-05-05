import numpy as np
from ..core.qvs import QVS

class QuantumAlgorithms:
    """
    Quantum Computation (Algorithms)
    Reuse: ASC (superposition) + ROTATE (unitary gates)
    Pattern: WEAVE operations creating interference.
    """
    def __init__(self, qvs):
        self.qvs = qvs

    def run_shor_pattern(self):
        """Demonstrate the Shor's algorithm pattern (interference)."""
        # 1. Initialize superposition
        psi_id = self.qvs.create_asc(size=3)
        self.qvs.SUPERPOSE(psi_id, [(0, 0, 0), (0, 0, 1), (0, 1, 0), (0, 1, 1), (1, 0, 0), (1, 0, 1), (1, 1, 0), (1, 1, 1)])
        
        # 2. Apply WEAVE for interference
        # In Shor's, this is the Quantum Fourier Transform.
        self.qvs.WEAVE(psi_id, None, np.pi/4)
        
        # 3. MEASURE (COLLAPSE)
        result = self.qvs.COLLAPSE(psi_id)
        return result

class QuantumErrorCorrection:
    """
    Quantum Error Correction (Surface Codes, Stabilizers)
    Reuse: NCB (entanglement) + WEAVE (syndrome extraction).
    Pattern: Distributed NCB networks where information is spread across many ASCs.
    """
    def __init__(self, qvs):
        self.qvs = qvs

    def create_stabilizer_bond(self):
        """Demonstrate a stabilizer code pattern using NCB."""
        # 1. Create two ASCs
        psi_id = self.qvs.create_asc(size=1)
        phi_id = self.qvs.create_asc(size=1)
        
        # 2. Forge NCB (entanglement) for redundancy
        # This creates a logical qubit across two physical ASCs.
        bonded_id = self.qvs.BOND(psi_id, phi_id, "bell")
        return bonded_id

class QuantumSimulation:
    """
    Quantum Simulation (Hamiltonian Evolution)
    Reuse: ASC (state vectors) + continuous ROTATE (time evolution).
    Pattern: Trotterized time evolution decomposes into local WEAVE and ROTATE.
    """
    def __init__(self, qvs):
        self.qvs = qvs

    def evolve_hamiltonian(self, time=1.0):
        """Demonstrate Hamiltonian evolution via lazy ROTATE accumulation."""
        # 1. Initialize state
        psi_id = self.qvs.create_asc(size=2)
        
        # 2. Apply ROTATE based on Hamiltonian (simplified)
        # H = X \otimes X
        H = np.array([
            [0, 0, 0, 1],
            [0, 0, 1, 0],
            [0, 1, 0, 0],
            [1, 0, 0, 0]
        ])
        # U = e^(-iHt)
        U = np.eye(4) * np.cos(time) - 1j * H * np.sin(time)
        self.qvs.ROTATE(psi_id, U)
        return psi_id

class QuantumCryptography:
    """
    Quantum Cryptography (QKD, Blind Computing)
    Reuse: NCB (correlation) + COLLAPSE (basis choice).
    Pattern: BB84/E91 alternating WEAVE and COLLAPSE.
    """
    def __init__(self, qvs):
        self.qvs = qvs

    def generate_qkd_key(self):
        """Demonstrate E91-style QKD using NCB."""
        # 1. Create entangled pair
        alice_id = self.qvs.create_asc(size=1)
        bob_id = self.qvs.create_asc(size=1)
        bonded_id = self.qvs.BOND(alice_id, bob_id, "bell")
        
        # 2. COLLAPSE to get correlated keys
        result = self.qvs.COLLAPSE(bonded_id)
        # result will be (0,0) or (1,1) with 50/50 probability
        return result

class QuantumFieldTheory:
    """
    Quantum Field Theory (Lattice Gauge Theory)
    Reuse: Lattices of ASCs (field configurations) + local NCBs (gauge constraints).
    Pattern: Massive ASC tensor networks with NCBs enforcing statistics.
    """
    def __init__(self, qvs):
        self.qvs = qvs

    def simulate_lattice_field(self):
        """Demonstrate a simple field configuration using ASC lattice."""
        # 1. Create a 2x2 lattice of ASCs
        lattice_ids = []
        for i in range(4):
            lattice_ids.append(self.qvs.create_asc(size=1))
        
        # 2. Apply NCBs between neighbors to represent field correlations
        # For simplicity, just bond the first two
        bonded_id = self.qvs.BOND(lattice_ids[0], lattice_ids[1], "bell")
        return bonded_id

class QuantumMachineLearning:
    """
    Quantum Machine Learning (Variational Circuits, Kernels)
    Reuse: ASC (feature maps) + parameterized ROTATE (trainable unitaries).
    Pattern: Gradient descent on WEAVE angles and BOND strengths.
    """
    def __init__(self, qvs):
        self.qvs = qvs

    def variational_step(self, params):
        """Demonstrate a single variational step in QML."""
        # 1. Initialize state (feature map)
        psi_id = self.qvs.create_asc(size=1)
        
        # 2. Apply parameterized ROTATE
        theta = params[0]
        U = np.array([
            [np.cos(theta/2), -np.sin(theta/2)],
            [np.sin(theta/2), np.cos(theta/2)]
        ])
        self.qvs.ROTATE(psi_id, U)
        
        # 3. MEASURE to get expectation value
        result = self.qvs.COLLAPSE(psi_id)
        return result
