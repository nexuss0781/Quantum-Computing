# Quantum-Computing / QAU — Detailed Claim and Deficiency Audit

> **Audit purpose**: distinguish what the repository demonstrably implements from what its language may suggest, identify scientific and engineering deficiencies, and define the work required before stronger claims are justified.

> **Bottom line**: The project is a promising **educational and research-prototype quantum-information simulator** with a growing C++ exact-state backend and several small toy-model experiments. It is **not yet** a quantum computer, a native quantum substrate, a scalable tensor-network engine, a GPU quantum engine, a quantum-field-theory solver, a quantum-gravity simulator, or a physical wormhole simulator.

## 1. Executive Verdict

The repository has real implementation value. It contains a functioning Python prototype, a C++20 dense state-vector backend, pybind11 bindings, deterministic small-register circuits, entropy and OTOC diagnostics, a discrete graph-geometry backreaction toy model, and an entanglement-assisted teleportation experiment. The current test results are also reproducible: five original Python tests pass, the CTest suite reports nine of nine tests passing, and the later Phase III–V integration harnesses pass.

Those green tests establish **internal consistency for selected examples**. They do not establish physical validity, scalability, computational advantage, or the existence of emergent spacetime. The core issue is not that the code is worthless; it is that the public language frequently exceeds the evidence. Several README statements use foundational or production-grade terms—“native on silicon,” “operating system layer,” “irreducible quantum computational primitives,” “synthesize any quantum system,” and “periodic table of quantum computation”—while the implementation is, in substance, a small classical simulator and a collection of toy demonstrations.

The correct present-day positioning is:

> **QAU is a C++/Python research prototype for exact small-register quantum-information experiments and explicitly labeled toy models of scrambling, entanglement, graph geometry, and teleportation-inspired protocols.**

That claim is defensible. Stronger claims require substantial scientific validation and engineering work described below.

## 2. What the Project Actually Contains

| Area | Demonstrable implementation | What the evidence does not establish |
|---|---|---|
| Python QVS | Sparse-looking ASC containers, phase operations, Bell-style bonds, collapse examples | A general quantum execution substrate, universal optimized simulator, or physical quantum hardware |
| C++ dense backend | Exact state vectors for small registers, Hadamard, $R_y$, $R_z$, CNOT, inverse circuits, Pauli operations | Scalable quantum computation; unrestricted state-vector simulation grows exponentially with qubit count [1] |
| MPS layer | A class named `MatrixProductState` and a vector of site matrices | A functioning MPS representation of entangled states; the current tensors are `2 x 1` product-state columns and no two-site gates, SVD, canonicalization, or truncation are implemented |
| Contraction layer | A method returning a sequential neighboring path | An optimal or cost-aware contraction planner; node indices and estimated costs are not used to choose the path |
| SIMD layer | OpenMP parallel loops with `simd` pragmas | Explicit AVX-512/NEON kernels, GPU compute, or a portable hardware-acceleration layer |
| QASM-R | Small instruction and VM layer | A validated compiler, formal bytecode specification, optimizer correctness, or production language |
| Entropy/Page curve | Exact reduced density matrices for small pure states and a seeded subsystem-partition toy model | Hawking radiation, black-hole evaporation, semiclassical gravity, or a physical Page curve |
| Graph backreaction | Triangulated weighted graph, angle-deficit and transport-curvature diagnostics, bounded edge updates | Einstein-equation evolution, Lorentzian spacetime, or a continuum quantum-gravity limit |
| Phase V | Three-qubit entanglement-assisted teleportation with fidelity and causality controls | A traversable wormhole, negative-energy stress tensor, ER=EPR proof, or faster-than-light communication |
| QFT/QML wrappers | Small demonstrations around Bell bonds, rotations, and hard-coded matrices | Shor’s algorithm, quantum error correction, lattice gauge theory, trainable QML, or general QFT simulation |

## 3. Claim Matrix: Defensible, Overstated, and Unsupported

### 3.1 Claims that are defensible today

| Claim | Evidence | Confidence |
|---|---|---:|
| The project has a working Python prototype for several small quantum-state operations. | The original Python test suite runs five tests successfully, covering superposition, interference, Bell-style entanglement, a GHZ-style scenario, and field wrappers. | High |
| The project has a C++20 exact dense-state backend for small registers. | `DenseQuantumState` stores a vector of $2^n$ complex amplitudes and enforces a 1–20 qubit limit. `QuantumCircuit` implements a small gate set and inverse execution. | High |
| The project can reproduce selected analytic quantum-information examples. | Bell-pair entropy, product-state entropy, norm preservation, reversible circuits, Bloch-vector recovery, and ideal teleportation examples are tested. | High |
| The project contains deterministic, reproducible toy models. | Seeded circuits, seeded Page-curve bookkeeping, deterministic graph evolution, and fixed six-state teleportation tests are present. | High |
| The project provides a useful platform for educational experiments. | The code is compact, directly inspectable, and accompanied by C++ and Python harnesses. | High |

### 3.2 Claims that are directionally reasonable but materially overstated

| Public or implied claim | Why it is overstated | Safer wording |
|---|---|---|
| “Moving beyond simulation” and “execute quantum primordials natively on silicon.” | The C++ code is still simulating finite-dimensional quantum states on a classical CPU. Changing the abstraction vocabulary does not remove classical simulation. | “Execute selected quantum-information primitives efficiently in a classical C++ simulator.” |
| “Native entanglement support.” | Entanglement is represented by amplitudes in a finite state vector or by toy correlation objects; no physical qubits, hardware coupling, or general entanglement-management runtime exists. | “Represent and diagnose entanglement in small simulated registers.” |
| “MPS with polynomial memory scaling.” | The current `MatrixProductState` class initializes independent `2 x 1` matrices. There are no bond legs beyond dimension one, no entangling operation, no decomposition, and no truncation. | “Prototype placeholder for a future MPS implementation.” |
| “Optimal tensor contraction path finder.” | `ContractionOptimizer::find_optimal_path` returns the input order as a chain and does not inspect `indices` or `estimated_cost`. | “Baseline sequential contraction-order stub.” |
| “Hardware acceleration,” “SIMD,” or “insane performance.” | The implementation uses OpenMP and compiler auto-vectorization. It contains no GPU backend and no explicit AVX-512, AVX2, NEON, CUDA, HIP, Vulkan, Metal, or OpenCL kernel. | “OpenMP-parallel CPU kernels with compiler auto-vectorization.” |
| “Quantum field applications.” | The field classes are thin demonstrations. For example, the QFT wrapper creates four ASCs but bonds only the first two; the QML wrapper performs one rotation and collapse; the Shor wrapper does not implement modular exponentiation, QFT, or period finding. | “Toy application examples inspired by QFT, QEC, QML, QKD, and algorithms.” |
| “Emergent spacetime” or “dynamic spacetime backreaction.” | The graph model applies a phenomenological edge-compression rule and measures graph observables. It does not solve the Einstein equations or define Lorentzian causal dynamics. | “Discrete graph-geometry response model.” |
| “Quantum black-hole dynamics” or “Page curve.” | The model keeps the global state closed and unitary and merely changes which qubits are labeled radiation. There is no horizon, field-theoretic radiation, mass loss, stress tensor, or Hawking process. | “Finite closed-system subsystem-entropy curve with evaporation bookkeeping.” |
| “ER=EPR wormhole synthesis.” | Phase V is ordinary coherent teleportation on three qubits. It does not construct geometry, calculate negative null energy, or establish a traversable channel in spacetime. | “ER=EPR-inspired entanglement-assisted teleportation toy model.” |
| “Synthesize any quantum system.” | The current gate set, state size, observables, noise model, Hamiltonian support, and tensor-network capabilities are far too limited for universality as a practical claim. | “Provide a foundation for selected small quantum-information experiments.” |

## 4. Detailed Scientific Deficiencies

### 4.1 No physical quantum substrate

The most important conceptual correction is that QAU is a **classical simulator**. A complex amplitude vector in C++ is a numerical representation of a quantum state; it is not a quantum state stored in physical matter. The phrase “native on silicon” can be used for a classical software implementation, but it must not be presented as if it creates quantum hardware or bypasses the classical cost of quantum simulation.

The dense backend stores $2^n$ complex amplitudes. This is the standard exact state-vector approach, whose memory and work scale exponentially in the unrestricted case [1]. The repository’s limit of 20 qubits is therefore an honest engineering guardrail, not evidence of scalability. The 4-million-state benchmark is a benchmark of a classical array kernel, not a demonstration of a quantum processor.

### 4.2 MPS implementation is not yet an MPS simulator

The file `cpp_engine/include/qau/mps.hpp` documents polynomial memory scaling, but every site tensor is initialized as a `2 x 1` matrix. Such tensors describe a product-state structure with bond dimension one. The class has only one meaningful operation: multiplying a single-site gate into one site matrix. There is no representation of left and right virtual bonds, no two-site gate update, no SVD, no canonical form, no truncation error, no expectation-value contraction, no sampling, and no conversion between dense and MPS states.

This is a **high-severity claim gap** because the MPS label currently suggests a scalability feature that the implementation does not provide. The fix is not a documentation change alone; it requires an actual tensor-network algorithm and tests comparing MPS results to dense references on small systems.

### 4.3 Tensor contraction optimizer is a stub

`ContractionOptimizer::find_optimal_path` ignores the dimensions encoded by node indices and ignores `estimated_cost`. It returns adjacent pairs in input order. It therefore cannot claim optimality, cost minimization, path search, treewidth reduction, or JIT contraction planning. It should be renamed to a baseline path generator until a cost model and search strategy are implemented.

### 4.4 Hardware acceleration claims exceed the code

The SIMD file uses `#pragma omp parallel for simd` and complex arithmetic. This may allow a compiler to vectorize loops, but it is not an explicit SIMD portability layer and it is not GPU compute. The build uses `-O3 -march=native -ffast-math -fopenmp`; these flags introduce portability and numerical-reproducibility risks. `-march=native` produces binaries tied to the build host. `-ffast-math` permits transformations that can violate strict floating-point assumptions, which is inappropriate for a quantum simulator unless numerical error is measured and bounded.

The CMake file also hard-codes `/usr/include/eigen3`, uses global `include_directories`, and relies on a legacy pybind11 CMake discovery path that emits a policy warning. The project needs portable target-based dependency discovery and separate safe/fast benchmark configurations.

### 4.5 Application-layer names are much stronger than their algorithms

The Python field layer is the clearest evidence of overclaiming:

| Wrapper | Actual operation | Missing for the stronger name |
|---|---|---|
| `run_shor_pattern` | Three-qubit uniform superposition, one global phase operation, collapse | Modular arithmetic, controlled modular exponentiation, QFT, period extraction, factoring validation |
| `create_stabilizer_bond` | One Bell bond between two one-qubit ASCs | Stabilizer tableau, syndrome extraction, encoded logical qubit, error model, decoder, threshold study |
| `evolve_hamiltonian` | One hard-coded $4\times4$ $X\otimes X$ unitary | General Hamiltonian interface, Trotter error control, time-step convergence, observables, sparse operators |
| `generate_qkd_key` | Bell-pair collapse | Basis selection, sifting, error estimation, privacy amplification, adversarial/noise model, security analysis |
| `simulate_lattice_field` | Four ASC objects with only the first two bonded | Lattice degrees of freedom, gauge links, Gauss constraints, action/Hamiltonian, observables, continuum or finite-size analysis |
| `variational_step` | One parameterized one-qubit rotation followed by collapse | Objective function, gradients, optimizer, parameter-shift validation, training curves, expressivity or noise analysis |

These wrappers are useful teaching examples, but they should be renamed as demonstrations or moved into an `examples/` namespace until substantive algorithms exist.

### 4.6 Phase III OTOC and Page-curve scope

The OTOC implementation is an exact finite-register diagnostic for a selected circuit and Pauli pair. It is not a Lyapunov-exponent measurement, a universal scrambling measure, or a black-hole diagnostic by itself. OTOC conventions vary, and the implementation uses a specific finite-state correlator and commutator proxy. The report should always state the operator choices, initial state, circuit family, depth, normalization, and finite-size effects.

The Page-curve model is more limited than its name suggests. The source code explicitly says that the full state remains closed and unitary and that “evaporation” only changes the bookkeeping partition. That is a valid educational model for subsystem entanglement, but it is not a Hawking process. It has no field modes, horizon, gravitational mass, stress-energy tensor, black-hole geometry, semiclassical radiation, or backreaction.

### 4.7 Phase IV backreaction is phenomenological, not Einstein dynamics

The Phase IV graph model uses an angle-deficit curvature proxy, an Ollivier-style transport observable, and a source-dependent edge-length compression rule. This is a valuable numerical laboratory. It does not vary a discrete Einstein-Hilbert or Regge action, propagate lapse/shift variables, enforce Hamiltonian and momentum constraints, define a Lorentzian metric, or demonstrate a continuum limit. The correct output is “geometry-like response under a specified rule,” not “emergent Einstein equations.”

A particularly important deficiency is that the source field is held fixed while geometry changes. There is no coupled matter evolution, stress-energy conservation, or feedback from geometry into matter. The model therefore tests one-way source-to-graph response, not self-consistent dynamical backreaction.

### 4.8 Phase V is teleportation, not wormhole synthesis

The Phase V code is scientifically the cleanest part because it explicitly labels itself a toy model. It prepares a Bell pair, performs Bell-basis analysis, and applies coherent corrections. The result is exactly the expected behavior of an entanglement-assisted teleportation protocol: ideal transfer with the correction information, a maximally mixed receiver without it, and a classical benchmark when the entanglement resource is removed.

That result is consistent with the teleportation literature [2] [3]. It is not evidence that the code has generated a wormhole. A traversable-wormhole claim would require at least a defined holographic model, a geometry or dual Hamiltonian, an inter-boundary coupling with a controlled energy condition, causal observables, and a demonstration that the protocol corresponds to the intended bulk channel. None of those are implemented.

## 5. Engineering Deficiencies

### 5.1 Portability and build reproducibility

| Deficiency | Consequence | Priority |
|---|---|---:|
| `-march=native` | Binary depends on build CPU; cannot distribute reproducibly | High |
| `-ffast-math` globally enabled | Can alter numerical semantics and hide stability problems | High |
| Hard-coded `/usr/include/eigen3` | Fails on many systems and package layouts | High |
| Global `include_directories` and link flags | Weak target isolation and dependency hygiene | Medium |
| CMake pybind11 policy warning | Fragile configuration path | Medium |
| Generated `cpp_engine/build` files committed | Bloats repository and mixes machine-specific artifacts with source | High |
| No evident CI matrix in the audited configuration | Passing locally does not establish Linux/macOS/Windows portability | High |
| No package metadata or install/export target | Users cannot reliably install the C++ library or Python module | Medium |

### 5.2 Numerical correctness and validation depth

The current tests are mostly deterministic happy-path tests. They are valuable regression tests, but they do not provide broad evidence of correctness. Missing categories include:

| Missing validation | Why it matters |
|---|---|
| Differential tests against NumPy/Qiskit/Cirq or an independently written reference | Detects shared implementation errors in the custom simulator |
| Randomized property tests over many seeds | Exposes gate-order, indexing, and normalization defects |
| Unitarity residuals for every gate family | Confirms that the implemented matrices preserve norms and inner products |
| Density-matrix positivity and trace tests | Required for entropy and channel diagnostics |
| Measurement-distribution statistical tests | A single sampled result cannot validate probabilities |
| Noise-channel and mixed-state tests | Real quantum-information experiments require channels, not only pure states |
| Scaling curves with memory, throughput, and error bars | A single timing point is not a performance characterization |
| Cross-platform builds | Required for claims about personal-computer usability |
| Long-run stability and tolerance sweeps | Necessary for claims about numerical robustness |
| Coverage measurement | “All tests pass” is not equivalent to meaningful code coverage |

### 5.3 Repository hygiene

The repository currently tracks generated CMake build outputs under `cpp_engine/build`. These include dependency files, object files, test logs, and binaries. They should be removed from version control and replaced with a `.gitignore`, reproducible build instructions, and CI artifacts generated outside the source tree.

The README also contains stale or contradictory roadmap information. It describes MPS, GPU support, autodiff, and production readiness as future work even though later commits add partial prototypes. Conversely, the README’s top-level language suggests capabilities that the roadmap itself marks as incomplete. The public documentation needs a single versioned capability matrix.

## 6. Severity-Ranked Deficiency Register

| ID | Deficiency | Severity | Claim affected | Required remedy |
|---|---|---:|---|---|
| D-01 | MPS class does not represent entangled MPS tensors | Critical | “Polynomial memory scaling,” tensor-network scalability | Implement bond dimensions, two-site updates, SVD truncation, contraction, and dense-reference tests |
| D-02 | “Quantum substrate/native quantum computation” language describes a classical simulator | Critical | QVS positioning and README overview | Reframe as classical quantum-information simulation; reserve hardware claims for actual hardware integration |
| D-03 | QFT/QML/QEC/Shor/QKD classes are demonstrations, not full algorithms | Critical | Quantum field applications and “synthesize any system” | Rename examples and implement domain-specific algorithms with independent validation |
| D-04 | No GPU backend despite Phase III wording | High | Hardware acceleration | Add CUDA/HIP/Vulkan/Metal backend or remove GPU language; benchmark against CPU baselines |
| D-05 | Contraction optimizer ignores cost and tensor indices | High | “Optimal contraction path” | Implement cost model and compare against opt-einsum/cotengra-style references |
| D-06 | Page curve is partition bookkeeping, not black-hole evaporation | High | Black-hole dynamics | Rename model and add a separate semiclassical/field-theoretic specification before stronger terminology |
| D-07 | Graph backreaction is one-way phenomenology, not coupled gravity | High | Emergent Einstein equations | Implement action-based discrete dynamics, constraints, matter evolution, and convergence studies |
| D-08 | Phase V is teleportation, not physical wormhole synthesis | High | ER=EPR/wormhole language | Maintain explicit toy-model label; add a true dual-model specification before geometric claims |
| D-09 | Global `-ffast-math`, `-march=native`, hard-coded Eigen path | High | Reliability and portability | Use target-based CMake, portable defaults, optional benchmark flags, and numerical tolerance tests |
| D-10 | Build artifacts committed to repository | High | Maintainability and reproducibility | Remove generated files, add `.gitignore`, add CI and clean build instructions |
| D-11 | Limited independent validation and statistical testing | High | “100% green” interpretation | Add differential, randomized, statistical, noise, and coverage tests |
| D-12 | Documentation inconsistency between README and later implementation | Medium | Public project credibility | Version capability claims and maintain one source-of-truth status document |

## 7. What “All Green” Actually Means

The phrase “all harnesses green” is valid only in this narrow sense: all currently registered tests passed for the selected inputs and tolerances. It does **not** mean that every quantum operation is correct, that all algorithms are implemented, that the model scales, or that the physics is validated.

A stronger project status should report at least four separate dimensions:

| Dimension | Current status |
|---|---|
| Regression status | Green for current deterministic tests |
| Algorithmic completeness | Partial; many application labels are demonstrations |
| Physical validity | Toy-model level; no quantum gravity or wormhole validation |
| Production readiness | Not ready; portability, packaging, CI, and artifact hygiene remain incomplete |

This distinction is essential. A test suite can prove that the implemented code behaves as coded on its test cases. It cannot prove that the code implements the broader theory suggested by its names.

## 8. Recommended Remediation Roadmap

### Stage A — Correct the claim surface immediately

The first action should be documentation integrity. Replace “native quantum substrate,” “quantum operating system,” “periodic table,” “synthesize any quantum system,” “GPU acceleration,” “MPS scaling,” “quantum field theory,” “black-hole dynamics,” and “wormhole synthesis” with precise status labels. Add a capability table with columns for implemented, experimental, planned, and unsupported.

### Stage B — Make the classical simulator technically solid

Remove tracked build artifacts, add a clean `.gitignore`, introduce target-based CMake, make Eigen discovery portable, separate safe numerical flags from benchmark flags, add sanitizers, and create CI on at least Linux and macOS. Add a Python packaging path and a documented clean-room build from a fresh checkout.

### Stage C — Build a real quantum-information core

Implement a typed gate and observable model, mixed states and channels, measurement distributions, controlled gates, tensor products, density-matrix evolution for small systems, and a reference-validation layer. Compare every primitive against an independent NumPy implementation. Add randomized state and circuit tests, property-based testing, statistical measurement tests, and coverage thresholds.

### Stage D — Implement a real MPS backend or remove the claim

If scalability is a genuine goal, implement three-index MPS tensors with left/right bond dimensions, canonicalization, two-site gate application, SVD truncation, truncation error accounting, expectation values, sampling, and dense cross-checks. Benchmark against exact states and publish memory/time/error curves. Until that work exists, call the current class a placeholder.

### Stage E — Separate physics toy models from physical theories

Place OTOCs, Page-like curves, graph curvature, and teleportation in a clearly marked `research_toy_models` namespace. Each model should have a mathematical specification, units or dimensionless conventions, finite-size analysis, convergence tests, and explicit “not implemented” lists. This preserves the ambitious vision without confusing a diagnostic proxy with a physical theory.

### Stage F — Earn stronger claims through reproducible evidence

A future claim such as “scalable tensor-network simulator” should require benchmark curves, independent reference agreement, multi-platform builds, and published resource limits. A future claim such as “holographic toy model” should require a defined dual Hamiltonian or tensor network, observables predicted on both sides, and finite-size/convergence analysis. A future claim such as “traversable-wormhole-inspired channel” should require a defined channel, causal controls, coupling parameter scans, and a clear distinction between boundary teleportation and bulk geometry.

## 9. Final Claim Recommendation

The project should currently advertise itself as follows:

> **QAU is an experimental C++20/Python framework for exact small-register quantum-information simulation and research toy models. It includes dense-state circuits, entropy and scrambling diagnostics, graph-based geometry experiments, and ER=EPR-inspired teleportation controls. Its tensor-network, field-theory, quantum-gravity, and wormhole components are exploratory prototypes rather than complete physical simulations.**

That statement is ambitious, accurate, and supported by the repository. Claims beyond it should be treated as roadmap goals until the deficiencies in this report are closed.

## References

[1] H. A. et al., “Full-State Vector Simulation of Quantum Time Evolution at Scale,” arXiv:2504.17881 (2026). [https://arxiv.org/html/2504.17881v2](https://arxiv.org/html/2504.17881v2)

[2] Bennett, C. H. et al., “Teleporting an Unknown Quantum State via Dual Classical and Einstein-Podolsky-Rosen Channels,” *Physical Review Letters* 70, 1895 (1993). [https://doi.org/10.1103/PhysRevLett.70.1895](https://doi.org/10.1103/PhysRevLett.70.1895)

[3] Brown, A. R. et al., “Quantum Gravity in the Lab. I. Teleportation by Size and Traversable Wormholes,” *PRX Quantum* 4, 010320 (2023). [https://doi.org/10.1103/PRXQuantum.4.010320](https://doi.org/10.1103/PRXQuantum.4.010320)

[4] Gao, P., Jafferis, D. L., and Wall, A. C., “Traversable Wormholes via a Double Trace Deformation,” arXiv:1608.05687. [https://arxiv.org/abs/1608.05687](https://arxiv.org/abs/1608.05687)

[5] Barrett, J. W., Oriti, D., and Williams, R. M., “Tullio Regge’s Legacy: Regge Calculus and Discrete Gravity,” arXiv:1812.06193. [https://arxiv.org/abs/1812.06193](https://arxiv.org/abs/1812.06193)

[6] van der Hoorn, P. et al., “Ollivier-Ricci Curvature Convergence in Random Geometric Graphs,” *Physical Review Research* 3, 013211 (2021). [https://doi.org/10.1103/PhysRevResearch.3.013211](https://doi.org/10.1103/PhysRevResearch.3.013211)
