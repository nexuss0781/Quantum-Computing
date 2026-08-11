# Project Event Horizon — Phase III Achievement Report

> **Phase**: Quantum Black Hole Dynamics & Information Scrambling  
> **Status**: Complete — all Phase III validation targets green  
> **Repository**: `nexuss0781/Quantum-Computing`  
> **Date**: August 2026  

## Executive Summary

Phase III has been implemented as a **scientifically explicit finite-dimensional toy-model suite** for quantum information scrambling, out-of-time-order correlators (OTOCs), and Page-curve-style evaporation. It is deliberately not described as a literal astrophysical black-hole or quantum-gravity solver. The model choices follow established quantum-information approaches: OTOCs as diagnostics of operator dynamics and scrambling [1], rapid mixing and information recovery in random-subsystem black-hole models [2], radiative random-unitary circuits with environment coupling [3], and qubit-transport Page-curve simulations [4].

The result is a deterministic C++20 dense-state backend with pybind11 access, exact reduced-density-matrix diagnostics for small registers, seeded brickwork circuits, OTOC evaluation, a closed-unitary evaporation bookkeeping model, CSV exports, and a combined diagnostic visualization.

## Implemented Components

| Component | File | Capability | Verification |
|---|---|---|---|
| Exact dense state backend | `cpp_engine/include/qau/quantum_dense.hpp` | 1–20 qubits, norm tracking, normalization, reversible state-vector evolution | Reversibility and norm property tests |
| Seeded scrambling circuits | `quantum_dense.hpp` | Hadamard, $R_y$, $R_z$, CNOT, inverse execution, deterministic brickwork layers | Reproducibility and unitary tests |
| OTOC analyzer | `quantum_dense.hpp` | Computes $F(t)=\langle\psi|W^\dagger(t)V^\dagger W(t)V|\psi\rangle$ and $C(t)=2(1-\operatorname{Re}F)$ for Hermitian-unitary operators | Analytic commuting and anticommuting cases |
| Entropy diagnostics | `cpp_engine/include/qau/black_hole.hpp` | Reduced density matrices, von Neumann entropy, purity | Product and Bell-state analytic tests |
| Evaporation toy model | `black_hole.hpp` | Closed global state, seeded scrambling, radiation/black-hole partition bookkeeping | Page-like rise-and-fall and global norm preservation |
| Python integration and plotting | `cpp_engine/tests/phase3_visualization.py` | OTOC scan, Page-curve scan, CSV export, PNG generation | CTest integration target and visual inspection |
| Research specification | `phase3_model_spec.md` | Mathematical definitions, acceptance criteria, references, scope limits | Used as implementation contract |

## Numerical Validation Results

The C++ Phase III harness passed all five test groups:

1. **Unitary reversibility and deterministic seeded circuits** passed. Forward evolution preserved norm, inverse evolution restored the computational-basis initial state, and identical seeds produced identical amplitudes.

2. **OTOC baseline and operator spreading** passed. A commuting operator pair produced $F=1$ and $C=0$. A two-CNOT propagation chain transformed the initially distant operator support so that the test pair produced $F=-1$ and $C=4$, with zero measured norm drift within the configured tolerance.

3. **Analytic entropy cases** passed. Product-state entropy and reduced-state purity matched $S=0$, $P=1$; a Bell pair reduced state matched $S=\ln 2$ and $P=1/2$.

4. **Page-curve toy model** passed on a six-qubit closed register. The measured radiation entropy sequence was

   `0.000000, 0.661147, 1.275771, 1.606783, 1.322868, 0.666302, 0.000000`.

   This produced an interior entropy maximum followed by a decline to zero when the radiation subsystem contained the entire globally pure register. Global norm error remained below the configured tolerance throughout.

5. **Reproducibility** passed. Repeated seeded evaporation runs produced identical entropy and purity diagnostics.

The Python harness generated the following OTOC commutator diagnostic sequence for seeded brickwork depth 0–8:

`0.000000, 0.000000, 0.000000, 0.640757, 0.200627, 2.521555, 1.185730, 2.271407, 2.627320`.

The initial plateau followed by nonzero operator-growth diagnostics is consistent with the expected causal-delay behavior of a local circuit, while the non-monotonic finite-size response is correctly treated as a toy-model diagnostic rather than a universal chaos law.

## Full Regression Status

The complete CTest suite passed **5 of 5 tests**:

| Test | Result |
|---|---|
| `QAUTests` | Passed |
| `Phase2Tests` | Passed |
| `Phase3Tests` | Passed |
| `EventHorizonPhase3Tests` | Passed |
| `EventHorizonPhase3PythonIntegration` | Passed |

The existing Phase 4 Python binding harness and Phase 5 lattice/QML integration harness were also rerun successfully after the Phase III changes. The combined Phase III plot was visually inspected and contains two readable panels: the finite-circuit OTOC diagnostic and the closed-unitary Page-curve toy model with radiation purity.

## Scientific Interpretation and Boundaries

These results demonstrate a working **quantum-information simulation platform**, not the discovery or simulation of an astrophysical black hole. The evaporation model keeps the complete state closed and unitary; “radiation” is a changing subsystem partition. It therefore tests Page-curve-like entanglement behavior without implementing Hawking radiation from a semiclassical metric, gravitational backreaction, Einstein equations, or a UV-complete theory of quantum gravity.

The dense backend is intentionally limited to small registers because exact state-vector and reduced-density calculations scale exponentially with qubit count. The seeded random circuit is a reproducible scrambling proxy, not a full Sachdev–Ye–Kitaev model, a continuum quantum field theory, or a proof of the fast-scrambling conjecture. OTOC values depend on the chosen initial state, operators, circuit geometry, and depth. The Page-like curve is a finite-size diagnostic and must be compared across ensembles and schedules before drawing stronger conclusions.

## Delivered Artifacts

| Artifact | Purpose |
|---|---|
| `cpp_engine/include/qau/quantum_dense.hpp` | Dense state-vector, circuit, and OTOC core |
| `cpp_engine/include/qau/black_hole.hpp` | Entropy diagnostics and evaporation toy model |
| `cpp_engine/tests/test_event_horizon_phase3.cpp` | C++ property, analytic, and regression harness |
| `cpp_engine/tests/phase3_visualization.py` | Python integration, CSV, and visualization harness |
| `cpp_engine/tests/phase3_otoc.csv` | OTOC scan output |
| `cpp_engine/tests/phase3_page_curve.csv` | Page-curve scan output |
| `cpp_engine/tests/phase3_otoc_page_curve.png` | Verified diagnostic figure |
| `phase3_model_spec.md` | Research-grounded model contract and acceptance criteria |
| `project_event_horizon_phase3_report.md` | This final achievement report |

## References

[1] Xu, S. and Swingle, B., “Scrambling Dynamics and Out-of-Time-Ordered Correlators in Quantum Many-Body Systems,” *PRX Quantum* 5, 010201 (2024). [https://doi.org/10.1103/PRXQuantum.5.010201](https://doi.org/10.1103/PRXQuantum.5.010201)

[2] Hayden, P. and Preskill, J., “Black holes as mirrors: quantum information in random subsystems,” *Journal of High Energy Physics* 2007, 120. [https://doi.org/10.1088/1126-6708/2007/09/120](https://doi.org/10.1088/1126-6708/2007/09/120)

[3] Weinstein, Z., Kelly, S. P., Marino, J., and Altman, E., “Scrambling Transition in a Radiative Random Unitary Circuit,” *Physical Review Letters* 131, 220404 (2023). [https://doi.org/10.1103/PhysRevLett.131.220404](https://doi.org/10.1103/PhysRevLett.131.220404)

[4] Chowdhury, T. A., Yu, K., Asaduzzaman, M., and Sufian, R. S., “Capturing the Page Curve and Entanglement Dynamics of Black Holes in Quantum Computers,” arXiv:2412.15180, version 3 (2025). [https://arxiv.org/abs/2412.15180](https://arxiv.org/abs/2412.15180)
