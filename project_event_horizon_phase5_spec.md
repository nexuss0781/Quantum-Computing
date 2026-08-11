# Project Event Horizon — Phase V Specification

## Scientific Scope

Phase V will implement an **ER=EPR-inspired quantum-information toy model**: two entangled registers, local scrambling, a controlled inter-register coupling, and state-transfer diagnostics. It will not claim to create a physical wormhole, negative-energy matter, or a macroscopic spacetime bridge.

The research basis is the holographic teleportation program of Brown et al., which describes information scrambled into one half of an entangled system, followed by weak coupling, then unscrambling in the other half [1]. The same work emphasizes that a clean geometric interpretation requires special operator-size behavior, while more generic systems can show information-transfer effects without a literal emergent wormhole geometry [1]. Gao, Jafferis, and Wall show in a holographic model that an inter-boundary interaction can produce negative average null energy and render an Einstein-Rosen bridge traversable, while preserving causality [2]. Susskind and Zhao frame wormhole passage as a quantum teleportation protocol that still requires classical communication outside the horizon [3].

## Protocol Model

The baseline model will use a three-register pure state:

- `M`: a one-qubit message state to be transferred.
- `L`: the left carrier/register.
- `R`: the right carrier/register.

The carriers begin in Bell-pair or multi-qubit entangled states. A seeded local scrambling circuit acts separately on `L` and `R`. A controlled coupling acts across the two carriers. The protocol then applies inverse scrambling and extracts the receiver-side message state.

The engine will compare three controls:

| Control | Expected behavior |
|---|---|
| Entanglement + coupling | High transfer fidelity in the ideal toy protocol |
| Entanglement without coupling | No receiver-side transfer beyond the protocol baseline |
| Coupling without entanglement | Low-fidelity/noisy transfer baseline |

The model will explicitly track whether classical correction bits are available. This prevents confusing ordinary quantum teleportation with superluminal communication: the protocol may transfer a state only when the classical feed-forward information is supplied, consistent with the literature [3].

## Diagnostics

For an input message density matrix $\rho_M$ and receiver output $\rho_R$, compute state fidelity

$$
F(\rho_M,\rho_R)=\left(\operatorname{Tr}\sqrt{\sqrt{\rho_M}\rho_R\sqrt{\rho_M}}\right)^2.
$$

For pure message states, this reduces to $F=\langle\psi_M|\rho_R|\psi_M\rangle$. The harness will also calculate:

1. Receiver Bloch-vector error for $X$, $Y$, and $Z$ input states.
2. Average fidelity over a deterministic six-state ensemble.
3. Global norm error and unitarity residual.
4. Entanglement negativity or mutual-information proxy across the two carriers before and after coupling.
5. Causal control comparison with and without classical correction bits.
6. Coupling-strength response and transfer-window scan.

## Acceptance Criteria

| Criterion | Required result |
|---|---|
| Bell-pair preparation | Carrier entanglement matches analytic Bell-state diagnostics. |
| Ideal transfer | Six-state average fidelity is above `1 - 1e-10` for the exact toy protocol. |
| No-entanglement control | Fidelity is materially lower than the ideal entangled case. |
| No-coupling control | Receiver does not acquire the message state without the transfer interaction/correction. |
| Causality | No protocol variant may transmit an unknown state using entanglement alone without classical feed-forward. |
| Unitarity | Global state norm error remains below `1e-10`. |
| Determinism | Identical seeds and parameters produce identical output diagnostics. |
| Limitations | Documentation distinguishes quantum teleportation/protocol equivalence from a physical traversable wormhole. |

## References

[1] Brown, A. R. et al., “Quantum Gravity in the Lab. I. Teleportation by Size and Traversable Wormholes,” *PRX Quantum* 4, 010320 (2023). [https://doi.org/10.1103/PRXQuantum.4.010320](https://doi.org/10.1103/PRXQuantum.4.010320)

[2] Gao, P., Jafferis, D. L., and Wall, A. C., “Traversable Wormholes via a Double Trace Deformation,” arXiv:1608.05687 (2019 version). [https://arxiv.org/abs/1608.05687](https://arxiv.org/abs/1608.05687)

[3] Susskind, L. and Zhao, Y., “Teleportation through the wormhole,” *Physical Review D* 98, 046016 (2018). [https://doi.org/10.1103/PhysRevD.98.046016](https://doi.org/10.1103/PhysRevD.98.046016)

[4] Bennett, C. H. et al., “Teleporting an Unknown Quantum State via Dual Classical and Einstein-Podolsky-Rosen Channels,” *Physical Review Letters* 70, 1895 (1993). [https://doi.org/10.1103/PhysRevLett.70.1895](https://doi.org/10.1103/PhysRevLett.70.1895)
