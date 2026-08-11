# Project Event Horizon — Phase IV Achievement Report

> **Phase**: Dynamic Spacetime Backreaction & Emergent Einstein-Equation Toy Models  
> **Status**: Complete — all Phase IV validation targets green  
> **Repository**: `nexuss0781/Quantum-Computing`  
> **Date**: August 2026  

## Executive Summary

Phase IV adds a mathematically explicit **discrete emergent-geometry backreaction laboratory** to the QAU engine. A triangulated weighted graph represents an effective spatial slice, a localized matter-energy field acts as a source, and a bounded relaxation rule updates the effective edge metric. The engine measures two independent curvature diagnostics: Regge-inspired triangle angle deficits and an Ollivier-style transport curvature computed through an exact small-network minimum-cost-flow solver.

This implementation is deliberately framed as a **toy model**, not as a full general-relativistic or quantum-gravity solver. Regge calculus provides the discrete-gravity motivation [1]. Weighted graph curvature and its continuum-convergence requirements motivate the transport diagnostic [2]. Holographic tensor-network work motivates studying matter and geometry through computationally accessible emergent models [3].

## Implemented Architecture

| Component | File | Capability | Validation |
|---|---|---|---|
| Weighted triangulated geometry | `cpp_engine/include/qau/backreaction.hpp` | Planar triangular grid, vertices, edges, faces, baseline and evolved lengths | Metric validity and flat-grid tests |
| Regge-inspired curvature | `backreaction.hpp` | Triangle areas, angles, interior angle deficits, curvature density | Regular planar lattice gives zero interior curvature |
| Matter source field | `backreaction.hpp` | Nonnegative point sources and radial Gaussian energy distributions | Parameter safety and conservation-style diagnostics |
| Transport curvature | `backreaction.hpp` | Weighted shortest paths, lazy random-walk measures, exact min-cost transport, Ollivier-style curvature | Finite deterministic transport-curvature regression |
| Backreaction evolution | `backreaction.hpp` | Bounded source-dependent edge compression, relaxation, metric repair | Stability, symmetry, positivity, and deterministic trajectory tests |
| Python interface | `cpp_engine/src/qau_bindings.cpp` | Geometry, source, evolution, edge and diagnostic access | Python integration harness |
| C++ validation | `tests/test_event_horizon_phase4.cpp` | Analytic, property, regression, and safety checks | All tests passed |
| Visualization and export | `tests/phase4_visualization.py` | Geometry plot, curvature response, transport curvature, update magnitude, CSV export | Generated plot visually inspected |

## Model Definition

For each triangle, the engine computes the angle deficit

$$
K_v = 2\pi - \sum_{t\supset v}\theta_{t,v}
$$

at interior vertices. The corresponding curvature density divides the deficit by a one-third triangle-area dual-cell estimate. Boundary vertices are excluded from the interior-curvature average because the current grid does not impose a boundary geodesic-curvature prescription.

The matter-to-geometry coupling is a controlled phenomenological law. For an edge joining vertices $i$ and $j$, the target length is

$$
\ell_{ij}^{\mathrm{target}} = \max\left(\ell_{ij}^{0}\,r_{\min},\nobreak\ell_{ij}^{0}e^{-g(\rho_i+\rho_j)/2}\right),
$$

and the relaxed update is

$$
\ell_{ij}^{t+1}=(1-\eta)\ell_{ij}^{t}+\eta\ell_{ij}^{\mathrm{target}}.
$$

After every update, a metric-repair pass enforces strict triangle inequalities. This guarantees a positive finite discrete metric under the tested parameter ranges, but it should not be confused with a derivation from the Einstein field equations.

For the transport diagnostic, each endpoint is assigned a lazy random-walk probability measure. Weighted shortest-path distances define the transport cost. An exact small-network min-cost-flow solver computes the Wasserstein transport cost, from which the edge curvature proxy is obtained as

$$
\kappa(x,y)=1-\frac{W_1(\mu_x,\mu_y)}{d(x,y)}.
$$

The implementation treats this as a graph observable and does not silently identify it with continuum Ricci curvature outside the conditions where convergence results apply [2].

## Validation Results

The Phase IV C++ harness passed all five validation groups:

1. **Flat triangulated baseline** passed. A regular $7\times7$ grid produced zero interior angle-deficit curvature within the configured numerical tolerance, remained metric-valid, and yielded a finite transport-curvature diagnostic.

2. **Localized source backreaction** passed. A centered Gaussian source produced a measurable non-flat response. Under the tested coupling, the total edge length decreased from the initial geometry while all edge lengths remained positive and all triangles remained valid.

3. **Deterministic geometry trajectories** passed. Identical initial geometries and source parameters produced identical average lengths, curvature diagnostics, transport diagnostics, and final edge metrics.

4. **Zero-source fixed point** passed. With no energy density, geometry remained unchanged and the interior curvature baseline stayed flat across evolution steps.

5. **Safety validation** passed. Negative coupling and negative source densities were rejected with explicit parameter errors.

The Python harness ran a $9\times9$ grid for ten evolution steps. Its principal outputs were:

| Quantity | Initial | Final |
|---|---:|---:|
| Total edge length | `234.50966799` | `224.50157187` |
| Mean absolute interior curvature | approximately `0` | `0.09625674` |
| Mean transport curvature | `0.0519` range | `0.05042741` |
| Metric validity | true | true |

The generated plot contains four panels: final source-compressed geometry, matter-to-geometry response, weighted transport curvature, and relaxed update magnitude. It was visually inspected after rendering; axes, titles, legends, node-source colors, and geometry lines were readable with no clipping or missing content.

## Full Regression Status

The complete CTest suite passed **7 of 7 tests**:

| Test | Result |
|---|---|
| `QAUTests` | Passed |
| `Phase2Tests` | Passed |
| `Phase3Tests` | Passed |
| `EventHorizonPhase3Tests` | Passed |
| `EventHorizonPhase4Tests` | Passed |
| `EventHorizonPhase3PythonIntegration` | Passed |
| `EventHorizonPhase4PythonIntegration` | Passed |

The legacy Phase 4 QASM-R/pybind11 harness and the earlier Phase 5 lattice/QML integration harness were also rerun successfully after the Phase IV binding changes.

## Scientific Boundaries

This phase does not implement full general relativity. It has no Lorentzian signature, lapse and shift fields, constraint propagation, dynamical causal cones, Einstein-Hilbert action, Regge action variation, quantum path integral, or continuum-limit proof. The source-dependent length law is an intentionally controlled backreaction experiment. The graph-curvature observable is useful for comparing geometry trajectories, but its interpretation depends on weighting, neighborhood scale, graph topology, and convergence regime.

The appropriate claim is therefore:

> **QAU now contains a reproducible discrete geometry laboratory in which localized information/energy sources deform an effective weighted spatial network and in which curvature-like observables can be measured and tested.**

It is not appropriate to claim that the engine has created physical spacetime, solved quantum gravity, or reproduced astrophysical black-hole dynamics.

## Delivered Artifacts

| Artifact | Purpose |
|---|---|
| `cpp_engine/include/qau/backreaction.hpp` | Discrete geometry, curvature, source fields, transport solver, and backreaction evolution |
| `cpp_engine/tests/test_event_horizon_phase4.cpp` | C++ Phase IV validation harness |
| `cpp_engine/tests/phase4_visualization.py` | Python integration and visualization harness |
| `cpp_engine/tests/phase4_backreaction.csv` | Exported geometry trajectory diagnostics |
| `cpp_engine/tests/phase4_backreaction.png` | Verified Phase IV visualization |
| `cpp_engine/tests/phase4_plot_review.md` | Visual inspection record |
| `project_event_horizon_phase4_spec.md` | Research basis, equations, scope, and acceptance criteria |
| `project_event_horizon_phase4_report.md` | This final achievement report |

## References

[1] Barrett, J. W., Oriti, D., and Williams, R. M., “Tullio Regge's legacy: Regge calculus and discrete gravity,” arXiv:1812.06193 (2019). [https://arxiv.org/abs/1812.06193](https://arxiv.org/abs/1812.06193)

[2] van der Hoorn, P. et al., “Ollivier-Ricci curvature convergence in random geometric graphs,” *Physical Review Research* 3, 013211 (2021). [https://doi.org/10.1103/PhysRevResearch.3.013211](https://doi.org/10.1103/PhysRevResearch.3.013211)

[3] Sahay, R., Lukin, M. D., and Cotler, J., “Emergent Holographic Forces from Tensor Networks and Criticality,” arXiv:2401.13595 (2024). [https://arxiv.org/html/2401.13595v1](https://arxiv.org/html/2401.13595v1)
