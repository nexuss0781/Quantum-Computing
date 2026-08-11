# Claim Audit Research Notes

## Quantum simulation scaling

Full state-vector simulation stores $2^n$ complex amplitudes for $n$ qubits, so memory and work scale exponentially in the unrestricted worst case. The relevant audit implication is that a 20-qubit cap in `DenseQuantumState` is an exact small-register limit, not scalable quantum computation. See the benchmark discussion in [1].

## Teleportation and causality

The Phase V protocol should be assessed as ordinary entanglement-assisted quantum teleportation with a coherent representation of the correction channel. The foundational teleportation protocol requires classical information in addition to shared entanglement [2]. The traversable-wormhole-inspired literature explicitly describes state transfer in terms of scrambling, coupling, and unscrambling, while distinguishing systems with a clean geometric interpretation from generic information-transfer effects [3]. Gao–Jafferis–Wall discuss negative average null energy and gravitational backreaction in a specific holographic model, which is not implemented by this repository [4].

## Holography and emergent geometry

A tensor network or graph observable can reproduce selected features of a holographic toy model without establishing a duality, a continuum gravitational limit, or an Einstein-equation solution. The audit must therefore distinguish “implements a diagnostic inspired by X” from “simulates X.”

## References

[1] H. A. et al., “Full-State Vector Simulation of Quantum Time Evolution at Scale,” arXiv:2504.17881 (2026). https://arxiv.org/html/2504.17881v2

[2] C. H. Bennett et al., “Teleporting an Unknown Quantum State via Dual Classical and Einstein-Podolsky-Rosen Channels,” Physical Review Letters 70, 1895 (1993). https://doi.org/10.1103/PhysRevLett.70.1895

[3] A. R. Brown et al., “Quantum Gravity in the Lab. I. Teleportation by Size and Traversable Wormholes,” PRX Quantum 4, 010320 (2023). https://doi.org/10.1103/PRXQuantum.4.010320

[4] P. Gao, D. L. Jafferis, and A. C. Wall, “Traversable Wormholes via a Double Trace Deformation,” arXiv:1608.05687. https://arxiv.org/abs/1608.05687
