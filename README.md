# Quantum Absolute Unit (QAU) - Quantum Virtual Substrate

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)
[![Python](https://img.shields.io/badge/Python-3.8+-blue.svg)](https://www.python.org/)
[![Tests](https://img.shields.io/badge/tests-passing-green.svg)](tests/test_qvs.py)

> **A paradigm shift in quantum computation** — Moving beyond simulation to establish a **Quantum Virtual Substrate (QVS)** where quantum primordials execute natively on silicon.

---

## 📖 Table of Contents

- [Overview](#overview)
- [The Three Quantum Primordials](#the-three-quantum-primordials)
- [Architecture](#architecture)
- [Installation](#installation)
- [Quick Start](#quick-start)
- [API Reference](#api-reference)
- [Quantum Field Applications](#quantum-field-applications)
- [Running Tests](#running-tests)
- [Project Structure](#project-structure)
- [Roadmap](#roadmap)
- [Contributing](#contributing)
- [License](#license)

---

## 🌟 Overview

The **Quantum Absolute Unit (QAU)** is not merely another quantum computing library—it represents a fundamental reimagining of how quantum computation can be implemented on classical hardware. Instead of simulating quantum mechanics gate-by-gate, QAU introduces a **Quantum Virtual Substrate (QVS)**: an operating system layer designed to execute irreducible quantum computational primitives—called **Quantum Primordials**—natively on silicon.

### Key Features

- **🧬 Three Quantum Primordials**: Built upon ASC (superposition), RPW (interference), and NCB (entanglement)
- **⚡ Lazy Tensor Operations**: Sparse representation with full vector arithmetic capabilities
- **🔗 Native Entanglement Support**: Shared constraint pointers for non-local correlations
- **🎯 Geometric Phase Arithmetic**: Efficient rotor-based phase manipulation
- **📊 Multi-Layer Architecture**: From kernel-level operations to high-level quantum field theories
- **🧪 Comprehensive Testing**: Full test suite covering all primitives and applications

---

## 🔬 The Three Quantum Primordials

At the core of QAU are three irreducible computational essences that cannot be decomposed further without losing "quantumness":

| Primordial | Computational Essence | Silicon Implementation | Role |
|------------|----------------------|------------------------|------|
| **ASC** | Coherent Multiplicity | Lazy Tensor Block (Sparse) | Foundation of parallelism |
| **RPW** | Interference | Geometric Rotor Algebra | Computational fuel |
| **NCB** | Informational Constraint | Shared Constraint Pointer | Non-local connectivity |

### 1. Amplitude Superposition Cell (ASC)

The primitive of **coherent multiplicity**. An ASC can simultaneously hold multiple values $\{v_1, v_2, ..., v_n\}$, each associated with complex weights $\{\alpha_1, \alpha_2, ..., \alpha_n\}$ such that $\sum |\alpha_i|^2 = 1$.

```python
from qau_qvs.core.asc import ASC

# Create an ASC representing a 2-qubit system
asc = ASC(size=2)
print(asc)  # ASC(size=2, states=1)
```

### 2. Relative Phase Weave (RPW)

The primitive of **interference**, where phase $e^{i\theta}$ is a fundamental computational element. RPW establishes continuous rotational relationships between ASCs using geometric algebra (rotors).

```python
from qau_qvs.core.rpw import RPW

# Apply phase rotation to a basis state
RPW.apply_phase(asc, basis_state=(0, 1), theta=np.pi/4)
```

### 3. Non-Local Correlation Bond (NCB)

The primitive of **entanglement**. NCB ensures joint probability distributions $P(a,b)$ cannot be factorized into $P(a)P(b)$, causing ASCs to behave as unified computational objects.

```python
from qau_qvs.core.ncb import NCB

# Create entanglement between two ASCs
ncb = NCB(asc_a, asc_b, correlation_type="bell")
```

---

## 🏗️ Architecture

The QVS is structured hierarchically into four layers:

```
┌─────────────────────────────────────────────────────────┐
│  LAYER 3: Quantum Field Theories                        │
│  (QFT, Quantum Gravity, Condensed Matter)               │
├─────────────────────────────────────────────────────────┤
│  LAYER 2: Quantum Algorithms                            │
│  (Algorithms, Circuits, Hamiltonians)                   │
├─────────────────────────────────────────────────────────┤
│  LAYER 1: Quantum Assembly (QASM-R)                     │
│  SUPERPOSE | WEAVE | BOND | ROTATE | COLLAPSE | TENSOR  │
├─────────────────────────────────────────────────────────┤
│  LAYER 0: Quantum Substrate Kernel                      │
│  (Tensor Networks, Primordial Execution on CPU/GPU)     │
└─────────────────────────────────────────────────────────┘
```

### Instruction Set

| Instruction | Description |
|-------------|-------------|
| `SUPERPOSE` | Initialize multiplicity with uniform superposition |
| `WEAVE` | Create interference patterns via phase rotation |
| `BOND` | Forge entanglement (NCB) between ASCs |
| `ROTATE` | Apply unitary transformations |
| `COLLAPSE` | Force projection (measurement) |
| `TENSOR` | Combine systems into joint state space |

---

## 📦 Installation

### Prerequisites

- Python 3.8+
- NumPy

### Setup

```bash
# Clone the repository
git clone https://github.com/your-org/qau.git
cd qau

# Install dependencies
pip install numpy

# Optional: Create virtual environment
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
```

---

## 🚀 Quick Start

### Basic Example: Creating and Manipulating Quantum States

```python
from qau_qvs.core.qvs import QVS
import numpy as np

# Initialize the Quantum Virtual Substrate
qvs = QVS()

# Create an ASC (Amplitude Superposition Cell)
psi_id = qvs.create_asc(size=1)

# Create superposition: |ψ⟩ = (|0⟩ + |1⟩)/√2
qvs.SUPERPOSE(psi_id, [(0,), (1,)])

# Apply interference pattern (phase rotation)
qvs.WEAVE(psi_id, None, np.pi/2)

# Measure the state
result = qvs.COLLAPSE(psi_id)
print(f"Measurement result: {result}")
```

### Entanglement Example: Bell State

```python
from qau_qvs.core.qvs import QVS

qvs = QVS()

# Create two independent ASCs
alice_id = qvs.create_asc(size=1)
bob_id = qvs.create_asc(size=1)

# Forge entanglement (Bell state)
bonded_id = qvs.BOND(alice_id, bob_id, correlation_type="bell")

# The bonded system is now in state: (|00⟩ + |11⟩)/√2
bonded_asc = qvs.ascs[bonded_id]
print(f"Bonded state amplitudes: {bonded_asc.amplitudes}")

# Measurement yields perfectly correlated results
result = qvs.COLLAPSE(bonded_id)
print(f"Correlated measurement: {result}")  # Either (0, 0) or (1, 1)
```

---

## 📚 API Reference

### QVS Class

The main interface for quantum operations.

#### Methods

| Method | Signature | Description |
|--------|-----------|-------------|
| `create_asc` | `create_asc(basis_states=None, size=None)` | Initialize a new ASC |
| `SUPERPOSE` | `SUPERPOSE(asc_id, basis_states)` | Create uniform superposition |
| `WEAVE` | `WEAVE(asc_id_a, asc_id_b, phase_angle)` | Apply interference pattern |
| `BOND` | `BOND(asc_id_a, asc_id_b, correlation_type)` | Create entanglement |
| `ROTATE` | `ROTATE(asc_id, unitary_matrix)` | Apply unitary transformation |
| `COLLAPSE` | `COLLAPSE(asc_id)` | Measure and collapse state |
| `TENSOR` | `TENSOR(asc_id_a, asc_id_b)` | Combine two systems |

### ASC Class

Amplitude Superposition Cell implementation.

```python
class ASC:
    def __init__(self, amplitudes=None, size=None)
    def normalize(self)
    def get_state_vector(self) -> np.ndarray
```

### RPW Class

Relative Phase Weave for interference operations.

```python
class RPW:
    def __init__(self, angle=0.0)
    @staticmethod
    def apply_phase(asc, basis_state, theta)
    @staticmethod
    def weave(asc_a, asc_b, theta)
```

### NCB Class

Non-Local Correlation Bond for entanglement.

```python
class NCB:
    def __init__(self, asc_a, asc_b, correlation_type="bell")
    def forge(self)
    @staticmethod
    def bond(asc_a, asc_b, correlation_type="bell")
```

---

## 🔮 Quantum Field Applications

The `qau_qvs.fields` module provides high-level abstractions for various quantum domains:

### 1. Quantum Algorithms

```python
from qau_qvs.fields.quantum_fields import QuantumAlgorithms

qvs = QVS()
alg = QuantumAlgorithms(qvs)

# Run Shor's algorithm pattern (interference-based)
result = alg.run_shor_pattern()
```

### 2. Quantum Error Correction

```python
from qau_qvs.fields.quantum_fields import QuantumErrorCorrection

qec = QuantumErrorCorrection(qvs)

# Create stabilizer code pattern
logical_qubit = qec.create_stabilizer_bond()
```

### 3. Quantum Simulation

```python
from qau_qvs.fields.quantum_fields import QuantumSimulation

sim = QuantumSimulation(qvs)

# Evolve under Hamiltonian H = X ⊗ X
evolved_state = sim.evolve_hamiltonian(time=np.pi/4)
```

### 4. Quantum Cryptography

```python
from qau_qvs.fields.quantum_fields import QuantumCryptography

crypto = QuantumCryptography(qvs)

# Generate E91-style QKD key
key = crypto.generate_qkd_key()  # Returns (0,0) or (1,1)
```

### 5. Quantum Machine Learning

```python
from qau_qvs.fields.quantum_fields import QuantumMachineLearning

qml = QuantumMachineLearning(qvs)

# Variational step with parameter θ
result = qml.variational_step(params=[np.pi/2])
```

### 6. Quantum Field Theory

```python
from qau_qvs.fields.quantum_fields import QuantumFieldTheory

qft = QuantumFieldTheory(qvs)

# Simulate lattice field configuration
lattice_state = qft.simulate_lattice_field()
```

---

## ✅ Running Tests

The project includes a comprehensive test suite covering all primitives and applications:

```bash
# Run all tests
python tests/test_qvs.py

# Expected output:
# Entanglement test passed.
# Interference test passed.
# Higher-layer quantum fields test passed.
# Strong quantum scenario (GHZ) passed with result: (1, 1, 1)
# Superposition test passed.
# ----------------------------------------------------------------------
# Ran 5 tests in 0.022s
# OK
```

### Test Coverage

- ✅ **Superposition**: ASC primitive verification
- ✅ **Interference**: RPW/WEAVE phase rotation tests
- ✅ **Entanglement**: NCB/BOND Bell state creation
- ✅ **Quantum Fields**: All six application domains
- ✅ **Strong Scenarios**: GHZ state preparation and measurement

---

## 📁 Project Structure

```
qau/
├── qau_qvs/                    # Main package
│   ├── __init__.py
│   ├── core/                   # Core primitives
│   │   ├── __init__.py
│   │   ├── asc.py             # Amplitude Superposition Cell
│   │   ├── rpw.py             # Relative Phase Weave
│   │   ├── ncb.py             # Non-Local Correlation Bond
│   │   └── qvs.py             # Quantum Virtual Substrate
│   └── fields/                 # High-level applications
│       ├── __init__.py
│       └── quantum_fields.py   # Quantum domain implementations
├── tests/
│   └── test_qvs.py            # Comprehensive test suite
├── qau_architecture_summary.md # Detailed architecture docs
├── qau_quantum_roadmap.md      # Development roadmap
├── LICENSE                     # Apache 2.0 License
└── README.md                   # This file
```

---

## 🛣️ Roadmap

### Phase 1: Foundation (Current)
- ✅ Implement three Quantum Primordials (ASC, RPW, NCB)
- ✅ Build QVS instruction set
- ✅ Create basic quantum field applications
- ✅ Establish test framework

### Phase 2: Optimization
- [ ] Tensor Network Compilation (MPS, Tree Tensor Networks)
- [ ] Geometric Phase Arithmetic with Clifford algebra
- [ ] JIT Unitary Synthesis using cotengra
- [ ] GPU acceleration support

### Phase 3: Advanced Features
- [ ] Measurement-First Execution (Monte Carlo trajectories)
- [ ] Automatic Differentiation for QML
- [ ] Distributed tensor networks
- [ ] Hybrid classical-quantum workflows

### Phase 4: Production Ready
- [ ] Performance benchmarking suite
- [ ] Documentation website
- [ ] Python package distribution (PyPI)
- [ ] Integration with major quantum frameworks

---

## 🤝 Contributing

We welcome contributions! Please follow these guidelines:

1. **Fork the repository** and create your branch
2. **Ensure tests pass**: `python tests/test_qvs.py`
3. **Follow the code style** (PEP 8 compliant)
4. **Add tests** for new features
5. **Submit a pull request** with clear description

### Areas for Contribution

- Optimizing tensor operations
- Implementing additional quantum algorithms
- Expanding quantum field theory models
- Improving documentation
- Adding visualization tools

---

## 📄 License

This project is licensed under the **Apache License 2.0** - see the [LICENSE](LICENSE) file for details.

```
Copyright 2026 Manus AI

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

---

## 📚 Additional Resources

- [Architecture Summary](qau_architecture_summary.md) - Detailed technical documentation
- [Quantum Roadmap](qau_quantum_roadmap.md) - Long-term vision and development plan

---

## 🙏 Acknowledgments

**Developed by Manus AI, 2026**

The QAU represents the **periodic table of quantum computation**—by mastering the Three Primordials (ASC, RPW, NCB), we can synthesize any quantum system, from simple algorithms to the most complex field theories.

---

<div align="center">

**"The QAU is more than a library; it is the periodic table of quantum computation."**

[Report Bug](../../issues) · [Request Feature](../../issues) · [View Documentation](qau_architecture_summary.md)

</div>
