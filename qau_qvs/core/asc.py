import numpy as np

class ASC:
    """
    Amplitude Superposition Cell (ASC)
    The primitive of coherent multiplicity.
    A lazy tensor block that stores only non-zero amplitudes.
    """
    def __init__(self, amplitudes=None, size=None):
        # amplitudes: dict mapping basis state (tuple of ints) to complex weight
        self.amplitudes = amplitudes if amplitudes is not None else {}
        self.size = size if size is not None else 0 # number of qubits/bits it represents
        if not self.amplitudes and self.size > 0:
            # default to |0...0>
            self.amplitudes[(0,) * self.size] = 1.0 + 0j

    def normalize(self):
        norm = np.sqrt(sum(abs(alpha)**2 for alpha in self.amplitudes.values()))
        if norm > 0:
            for k in self.amplitudes:
                self.amplitudes[k] /= norm
        return self

    def __repr__(self):
        return f"ASC(size={self.size}, states={len(self.amplitudes)})"

    def get_state_vector(self):
        """Returns the full state vector (for small systems/debugging)."""
        dim = 2**self.size
        vec = np.zeros(dim, dtype=complex)
        for state, weight in self.amplitudes.items():
            idx = 0
            for i, bit in enumerate(reversed(state)):
                idx += bit * (2**i)
            vec[idx] = weight
        return vec
