# Project Event Horizon — Phase IV Specification

## Scientific Scope

Phase IV will implement a **discrete emergent-geometry toy model** in which a weighted graph represents an effective spatial slice, localized matter-energy density acts as a source, and edge lengths evolve through a controlled backreaction rule. It will not claim to solve the full Einstein field equations, reproduce Lorentzian causal structure, or establish quantum gravity.

The design is informed by three complementary research directions. Regge calculus discretizes gravitational geometry and has been used in classical relativity and quantum-gravity approaches [1]. Ollivier-Ricci curvature provides a transport-based graph curvature with a rigorous continuum-convergence result for suitable weighted random geometric graphs and mesoscopic neighborhoods [2]. Holographic tensor-network models provide computationally accessible toy dualities in which emergent bulk matter can exhibit interactions with AdS-like gravitational features [3].

## Model Definition

Let a graph have vertices $i$ and positive edge lengths $\ell_{ij}$. Each vertex carries a nonnegative matter-energy density $\rho_i$. The engine will compute a local graph curvature proxy using weighted triangle angle deficits on triangulated planar lattices:

$$
K_f = 2\pi - \sum_{t\supset f} \theta_{t,f},
$$

where $\theta_{t,f}$ is the angle at vertex $f$ in triangle $t$. Vertex curvature is the area-weighted average of incident face deficits.

For a controlled toy backreaction law, define a local target curvature

$$
K_i^{\mathrm{target}} = \Lambda + G\rho_i,
$$

and update edge lengths through a bounded relaxation step driven by curvature mismatch:

$$
\ell_{ij}^{(t+1)} = \operatorname{clip}\left(\ell_{ij}^{(t)}\left[1 - \eta\,\bar{\rho}_{ij}\right],\nobreak\ell_{\min},\ell_{\max}\right),
$$

with a smoothing and normalization stage that preserves positive lengths. This is an explicitly phenomenological discrete evolution law designed to test matter-to-geometry coupling and stability, not a derivation of Einstein's equations.

The implementation will include a second diagnostic based on weighted Ollivier-style transport curvature on the graph. It will be used as a comparative observable rather than silently conflated with the triangle-deficit curvature, because discrete curvature definitions are not unique and may disagree outside their convergence regimes [2].

## Acceptance Criteria

| Criterion | Required result |
|---|---|
| Flat-lattice baseline | A regular triangulated periodic or planar lattice has near-zero interior curvature under the chosen boundary convention. |
| Positive-source response | Localized positive matter density produces a reproducible, bounded curvature/edge-length response. |
| Radial response | A symmetric source produces a symmetric geometry change within numerical tolerance. |
| Stability | Edge lengths remain finite and positive for all tested time steps. |
| Conservation-style diagnostic | The harness reports total source density, average edge length, curvature norm, and update residual at every step. |
| Determinism | Identical initial geometry, source field, and parameters reproduce identical trajectories. |
| Regression safety | All prior QAU and Event Horizon Phase III tests remain green. |
| Scientific honesty | Reports clearly distinguish the toy backreaction law from full general relativity. |

## References

[1] Barrett, J. W., Oriti, D., and Williams, R. M., “Tullio Regge's legacy: Regge calculus and discrete gravity,” arXiv:1812.06193 (2019). [https://arxiv.org/abs/1812.06193](https://arxiv.org/abs/1812.06193)

[2] van der Hoorn, P. et al., “Ollivier-Ricci curvature convergence in random geometric graphs,” *Physical Review Research* 3, 013211 (2021). [https://doi.org/10.1103/PhysRevResearch.3.013211](https://doi.org/10.1103/PhysRevResearch.3.013211)

[3] Sahay, R., Lukin, M. D., and Cotler, J., “Emergent Holographic Forces from Tensor Networks and Criticality,” arXiv:2401.13595 (2024). [https://arxiv.org/html/2401.13595v1](https://arxiv.org/html/2401.13595v1)
