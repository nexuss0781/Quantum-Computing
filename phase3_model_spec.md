# Project Event Horizon — Phase III Model Specification

## Scope and Scientific Position

Phase III will implement **validated toy models of quantum information dynamics**, not a literal astrophysical black-hole solver. The implementation will use finite-dimensional unitary circuits and controlled evaporation channels to study information spreading, operator growth, and radiation entropy. These are computational analogues inspired by black-hole information models; they do not establish quantum gravity or reproduce semiclassical general relativity.

The primary reference points are the scrambling and OTOC tutorial by Xu and Swingle, which frames scrambling through operator dynamics and gives numerically tractable examples including random circuits [1], the Hayden–Preskill model of rapid information recovery from a sufficiently old unitary black hole [2], and the radiative random-unitary-circuit work that relates OTOC percolation and decoding across an environment-coupling transition [3]. A recent quantum-computing study implements a qubit-transport evaporation model with random unitary scrambling and Rényi-entropy measurements [4].

## Mathematical Models

### 1. Unitary Scrambling Circuit

A register of `N` qubits evolves under a sequence of unitary gates. The default benchmark will use brickwork two-qubit gates, with deterministic seeded pseudo-random parameters so results are reproducible. The state vector must preserve norm within a configured numerical tolerance.

### 2. Out-of-Time-Order Correlator

For a pure initial state $|\psi\rangle$, operators $W$ and $V$, and unitary evolution $U(t)$, define

$$
W(t) = U^\dagger(t) W U(t),
\qquad
F(t) = \langle \psi | W^\dagger(t) V^\dagger W(t) V | \psi \rangle.
$$

For Hermitian Pauli operators with $W^2=V^2=I$, the magnitude and real part of `F(t)` are the primary diagnostics. The implementation will additionally expose the commutator diagnostic

$$
C(t) = 2\left(1 - \operatorname{Re}F(t)\right),
$$

for the normalized Hermitian-unitary case. The test harness will explicitly distinguish the OTOC from entanglement entropy: OTOC decay indicates operator non-commutativity/spreading, while entropy measures state bipartition entanglement [1].

### 3. Evaporation and Page-Curve Toy Model

The evaporation model partitions a closed pure register into black-hole qubits `B` and radiation qubits `R`. At each step, a seeded unitary acts on the active black-hole subsystem and one or more qubits are transferred to `R`. The radiation entropy is computed from the reduced density matrix of `R`.

The expected qualitative criterion is a **Page-like rise-and-fall** in radiation entropy for a globally pure state: entropy rises while radiation is a smaller subsystem and decreases after radiation becomes the larger subsystem, subject to finite-size effects and the selected circuit/transfer schedule. The implementation will also compute purity and the final global norm, because a plot alone is not sufficient evidence of unitary information preservation.

### 4. Optional Hayden–Preskill Recovery Diagnostic

Where computationally tractable, the harness will include a reference qubit entangled with the initial black-hole register and measure mutual information or reconstruction fidelity after radiation transfer. This is a diagnostic of the toy channel, not a claim that a physical black hole has been simulated.

## Acceptance Criteria

| Criterion | Required result |
|---|---|
| Deterministic reproducibility | Same seed and configuration produce identical scalar diagnostics and state hashes. |
| Unitarity | Norm error remains below `1e-10` for double-precision dense tests. |
| OTOC baseline | At `t = 0`, the correlator matches the analytically evaluated operator/state baseline. |
| OTOC causal growth | A local circuit produces delayed deviation from the baseline relative to a nonlocal all-to-all control, for the chosen operator separation. |
| Entropy correctness | Bell and product-state reduced entropies match analytic values within `1e-10`. |
| Page-like behavior | A controlled evaporation configuration produces a documented radiation-entropy peak and subsequent decline, with no violation of global purity. |
| Regression safety | Existing Phase 1–II and previous Event Horizon tests remain green. |
| Scientific honesty | Documentation labels finite random-circuit models as toy analogues and reports assumptions/limitations. |

## Sources

[1] Xu, S. and Swingle, B., “Scrambling Dynamics and Out-of-Time-Ordered Correlators in Quantum Many-Body Systems,” *PRX Quantum* 5, 010201 (2024), https://doi.org/10.1103/PRXQuantum.5.010201.

[2] Hayden, P. and Preskill, J., “Black holes as mirrors: quantum information in random subsystems,” *JHEP* 2007, 120, https://doi.org/10.1088/1126-6708/2007/09/120.

[3] Weinstein, Z. et al., “Scrambling Transition in a Radiative Random Unitary Circuit,” *Physical Review Letters* 131, 220404 (2023), https://doi.org/10.1103/PhysRevLett.131.220404.

[4] Chowdhury, T. A. et al., “Capturing the Page Curve and Entanglement Dynamics of Black Holes in Quantum Computers,” arXiv:2412.15180 (v3, 2025), https://arxiv.org/abs/2412.15180.
