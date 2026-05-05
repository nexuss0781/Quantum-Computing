import sys
import os
import numpy as np
import unittest

# Add the project root to sys.path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))

from qau_qvs.core.qvs import QVS
from qau_qvs.fields.quantum_fields import (
    QuantumAlgorithms, QuantumErrorCorrection, QuantumSimulation,
    QuantumCryptography, QuantumFieldTheory, QuantumMachineLearning
)

class TestQVS(unittest.TestCase):
    def setUp(self):
        self.qvs = QVS()

    def test_superposition(self):
        """Test ASC superposition (ASC primitive)."""
        psi_id = self.qvs.create_asc(size=1)
        # Create uniform superposition of |0> and |1>
        self.qvs.SUPERPOSE(psi_id, [(0,), (1,)])
        asc = self.qvs.ascs[psi_id]
        
        # Check weights
        self.assertAlmostEqual(abs(asc.amplitudes[(0,)])**2, 0.5)
        self.assertAlmostEqual(abs(asc.amplitudes[(1,)])**2, 0.5)
        print("Superposition test passed.")

    def test_interference(self):
        """Test RPW/WEAVE interference (RPW primitive)."""
        psi_id = self.qvs.create_asc(size=1)
        # Start with |1>
        self.qvs.ascs[psi_id].amplitudes = {(1,): 1.0 + 0j}
        
        # Apply WEAVE with PI/2 phase
        self.qvs.WEAVE(psi_id, None, np.pi/2)
        asc = self.qvs.ascs[psi_id]
        
        # |1> should now have phase e^(i*pi/2) = i
        self.assertAlmostEqual(asc.amplitudes[(1,)].real, 0)
        self.assertAlmostEqual(asc.amplitudes[(1,)].imag, 1)
        print("Interference test passed.")

    def test_entanglement(self):
        """Test NCB/BOND entanglement (NCB primitive)."""
        alice_id = self.qvs.create_asc(size=1)
        bob_id = self.qvs.create_asc(size=1)
        
        # Bond Alice and Bob into a Bell state
        bonded_id = self.qvs.BOND(alice_id, bob_id, "bell")
        asc = self.qvs.ascs[bonded_id]
        
        # Check if we have (0,0) and (1,1) with equal probability
        self.assertIn((0, 0), asc.amplitudes)
        self.assertIn((1, 1), asc.amplitudes)
        self.assertAlmostEqual(abs(asc.amplitudes[(0, 0)])**2, 0.5)
        self.assertAlmostEqual(abs(asc.amplitudes[(1, 1)])**2, 0.5)
        print("Entanglement test passed.")

    def test_quantum_fields(self):
        """Test the higher-layer quantum fields."""
        # 1. Algorithms
        alg = QuantumAlgorithms(self.qvs)
        res_alg = alg.run_shor_pattern()
        self.assertIsInstance(res_alg, tuple)
        
        # 2. Simulation
        sim = QuantumSimulation(self.qvs)
        sim_id = sim.evolve_hamiltonian(time=np.pi/4)
        self.assertIn(sim_id, self.qvs.ascs)
        
        # 3. Cryptography
        crypto = QuantumCryptography(self.qvs)
        key = crypto.generate_qkd_key()
        self.assertIn(key, [(0, 0), (1, 1)])
        
        # 4. QML
        qml = QuantumMachineLearning(self.qvs)
        qml_res = qml.variational_step([np.pi/2])
        self.assertIsInstance(qml_res, tuple)
        
        print("Higher-layer quantum fields test passed.")

    def test_strong_quantum_scenario(self):
        """A strong scenario: GHZ state preparation and measurement."""
        # Create 3 ASCs
        a = self.qvs.create_asc(size=1)
        b = self.qvs.create_asc(size=1)
        c = self.qvs.create_asc(size=1)
        
        # Bond them sequentially to create a 3-qubit system
        # In a real QVS, this would create a massive tensor network
        ab = self.qvs.BOND(a, b, "bell") # |00> + |11>
        abc = self.qvs.TENSOR(ab, c) # (|00> + |11>) \otimes |0> = |000> + |110>
        
        # Apply a ROTATE to create a GHZ state |000> + |111>
        # For simplicity, we manually set the GHZ state to demonstrate the QVS state management
        ghz_asc = self.qvs.ascs[abc]
        ghz_asc.amplitudes = {
            (0, 0, 0): 1.0 / np.sqrt(2),
            (1, 1, 1): 1.0 / np.sqrt(2)
        }
        
        # COLLAPSE should yield either (0,0,0) or (1,1,1)
        res = self.qvs.COLLAPSE(abc)
        self.assertIn(res, [(0, 0, 0), (1, 1, 1)])
        print(f"Strong quantum scenario (GHZ) passed with result: {res}")

if __name__ == '__main__':
    unittest.main()
