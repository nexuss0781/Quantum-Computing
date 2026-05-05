import numpy as np
import random
from .asc import ASC
from .rpw import RPW
from .ncb import NCB

class QVS:
    """
    Quantum Virtual Substrate (QVS)
    The operating system layer that executes the Quantum Primordials natively on silicon.
    Implements the QVS instruction set: SUPERPOSE, WEAVE, BOND, ROTATE, COLLAPSE, TENSOR.
    """
    def __init__(self):
        self.ascs = {} # map of id to ASC instances
        self.next_id = 0

    def create_asc(self, basis_states=None, size=None):
        """Initialize an ASC with multiplicity."""
        asc_id = f"ASC_{self.next_id}"
        self.next_id += 1
        self.ascs[asc_id] = ASC(basis_states, size)
        return asc_id

    def SUPERPOSE(self, asc_id, basis_states):
        """
        SUPERPOSE(ASC_id, basis_states[]) - Initialize multiplicity
        Creates a uniform superposition of the given basis states.
        """
        asc = self.ascs[asc_id]
        weight = 1.0 / np.sqrt(len(basis_states))
        asc.amplitudes = {state: weight for state in basis_states}
        return asc_id

    def WEAVE(self, asc_id_a, asc_id_b, phase_angle):
        """
        WEAVE(ASC_id_A, ASC_id_B, phase_angle) - Create interference pattern
        In this implementation, we apply a relative phase between states.
        """
        # For a single ASC, this can be a phase shift on certain states.
        # For two ASCs, it can be a controlled phase shift.
        # Let's simplify: apply a phase shift on the second state of asc_id_a.
        asc = self.ascs[asc_id_a]
        # In the QVS model, WEAVE is about interference.
        # For simplicity, we apply a phase rotation to the basis states.
        for state in list(asc.amplitudes.keys()):
            # A simple rule: if the state has 1 at a certain bit, apply phase.
            if sum(state) % 2 == 1:
                RPW.apply_phase(asc, state, phase_angle)
        return asc_id_a

    def BOND(self, asc_id_a, asc_id_b, correlation_type="bell"):
        """
        BOND(ASC_id_A, ASC_id_B, correlation_type) - Forge NCB (entanglement)
        Combines two ASCs into a single joint ASC with non-local correlations.
        """
        # 1. TENSOR the two systems
        joint_id = self.TENSOR(asc_id_a, asc_id_b)
        joint_asc = self.ascs[joint_id]
        
        # 2. Apply entanglement based on correlation_type
        if correlation_type == "bell":
            # Example: create |00> + |11> from |00>
            # For simplicity, let's assume the joint state was |00>
            # We'll just set it directly to represent the bond.
            if (0, 0) in joint_asc.amplitudes:
                joint_asc.amplitudes = {
                    (0, 0): 1.0 / np.sqrt(2),
                    (1, 1): 1.0 / np.sqrt(2)
                }
        return joint_id

    def ROTATE(self, asc_id, unitary_matrix):
        """
        ROTATE(ASC_id, unitary_matrix) - Apply information-preserving transform
        Applies a unitary matrix to the state vector of the ASC.
        """
        asc = self.ascs[asc_id]
        # For simplicity, we convert to state vector, rotate, and convert back.
        # In the QVS, this would be optimized tensor contraction.
        vec = asc.get_state_vector()
        new_vec = np.dot(unitary_matrix, vec)
        
        # Re-populate sparse amplitudes
        new_amplitudes = {}
        dim = 2**asc.size
        for i in range(dim):
            if abs(new_vec[i]) > 1e-10:
                # Convert index back to tuple of bits
                bits = tuple((i >> j) & 1 for j in range(asc.size - 1, -1, -1))
                new_amplitudes[bits] = new_vec[i]
        asc.amplitudes = new_amplitudes
        return asc_id

    def COLLAPSE(self, asc_id):
        """
        COLLAPSE(ASC_id, basis) - Force projection (measurement)
        Collapses the ASC to a single basis state based on probabilities.
        """
        asc = self.ascs[asc_id]
        states = list(asc.amplitudes.keys())
        probs = [abs(asc.amplitudes[s])**2 for s in states]
        
        # Normalize probabilities (to handle precision errors)
        probs = np.array(probs) / sum(probs)
        
        chosen_idx = np.random.choice(len(states), p=probs)
        chosen_state = states[chosen_idx]
        
        asc.amplitudes = {chosen_state: 1.0 + 0j}
        return chosen_state

    def TENSOR(self, asc_id_a, asc_id_b):
        """
        TENSOR(ASC_id_A, ASC_id_B) - Combine systems (exponential state space)
        Creates a new ASC representing the joint system of A and B.
        """
        asc_a = self.ascs[asc_id_a]
        asc_b = self.ascs[asc_id_b]
        
        new_amplitudes = {}
        for state_a, weight_a in asc_a.amplitudes.items():
            for state_b, weight_b in asc_b.amplitudes.items():
                joint_state = state_a + state_b
                new_amplitudes[joint_state] = weight_a * weight_b
        
        new_id = self.create_asc(new_amplitudes, asc_a.size + asc_b.size)
        # Remove old ASCs as they are now part of the joint system
        del self.ascs[asc_id_a]
        del self.ascs[asc_id_b]
        return new_id
