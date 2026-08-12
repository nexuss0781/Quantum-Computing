# Research.md
## Quantum Absolute Units from the Vacuum Ground State
### An auditable C++ proposal for loss- and phase-noise-aware squeezed-vacuum sensing

**Project:** Quantum Atomic Unit (`QAU`)  
**Repository:** `nexuss0781/Quantum-Computing`  
**Research status:** Model-level mathematics and C++ derivation implementation completed; physical hardware validation not yet performed.  
**Approval purpose:** Give an independent developer enough mathematics, unit definitions, source locations, tests, evidence rules, and failure criteria to reproduce or reject the proposal.

---

## 1. Executive result

This document defines a family of **Quantum Absolute Units** whose reference is the vacuum ground state of a declared bosonic field mode. It also gives a concrete applied problem:

> **Given a squeezed-vacuum sensor with calibrated optical transmissivity and phase-diffusion variance, choose the squeezing strength that minimizes the measured signal-quadrature variance.**

The solution is derived analytically and implemented in C++:

$$
\boxed{r_\star=\frac12\operatorname{artanh}\left(e^{-2\sigma_\phi^2}\right)}
$$

for nonzero Gaussian phase-diffusion variance $\sigma_\phi^2$, with predicted minimum variance

$$
\boxed{V_{\min,\star}=\frac{1-\eta}{2}+\frac{\eta}{2}\sqrt{1-e^{-4\sigma_\phi^2}}}
$$

where $\eta$ is optical transmissivity. The result is exact for the declared single-mode Gaussian model and is verified by a C++ derivation harness.

This is a real engineering solution to a defined quantum-sensing control problem. It is **not** evidence that QAU has discovered a new physical law, created a physical vacuum, solved the measurement problem, or demonstrated a laboratory quantum advantage. Those are separate approval gates requiring measured data.

---

## 2. Why this is the selected problem

Squeezed vacuum is a real quantum resource used in precision measurement. Squeezing reduces noise in one field quadrature while increasing noise in the conjugate quadrature; optical loss and phase noise reduce the benefit. LIGO describes this tradeoff and reports the use of frequency-dependent squeezing to improve detector sensitivity [1]. An open engineering difficulty is not the existence of the ideal formula; it is selecting and maintaining the correct squeezing strength and phase when loss and phase diffusion vary in a real sensor.

The proposal therefore does not claim to solve an unsolved law of nature. It addresses an unresolved **deployment-control problem** with a fully specified model, a controller, and a physical test protocol. The appropriate novelty test is whether the QAU controller improves held-out physical sensor data relative to fixed-squeezing and independently implemented numerical baselines.

The loss/noise problem is experimentally relevant. Published work has studied how detection loss and noise affect squeezing-based sensing and demonstrated sub-shot-noise phase sensitivity under substantial loss [2]. QAU’s role is to make the vacuum reference, unit composition, analytic policy, resource budget, and evidence packet executable and auditable.

---

## 3. Quantum Absolute Unit registry

“Absolute” means that the unit has a fixed reference state and invariant contract inside the QAU engine. It does not claim that software has identified the ontological bottom of the universe.

| ID | Unit | Role | Mathematical object | Invariant |
|---|---|---|---|---|
| `VAC-0` | Vacuum Reference Unit | Ground-state baseline | $|0\rangle$, $a|0\rangle=0$ | $\langle n\rangle=0$, $V_X=V_P=1/2$ |
| `MOD-1` | Bosonic Mode Unit | One declared field mode | $\mathcal H_N=\operatorname{span}\{|0\rangle,\ldots,|N-1\rangle\}$ | normalized state; cutoff explicit |
| `EXC-1` | Excitation Unit | Occupation above vacuum | $n=a^\dagger a$ | $n\ge0$ |
| `QX-1` | X-Quadrature Unit | Signal/noise coordinate | $X=(a+a^\dagger)/\sqrt2$ | participates in uncertainty bound |
| `QP-1` | P-Quadrature Unit | Conjugate coordinate | $P=(a-a^\dagger)/(i\sqrt2)$ | $V_XV_P\ge1/4$ |
| `PHI-1` | Phase Reference Unit | Local-oscillator/squeeze orientation | $R(\phi)$ | rotation preserves energy/spectrum |
| `SQ-1` | Squeeze Resource Unit | Noise redistribution | $S(r,\phi)$ | $V_XV_P\ge1/4$ |
| `LOSS-1` | Loss Channel Unit | Vacuum-port coupling | $\mathcal L_\eta$ | CPTP model; vacuum mixing |
| `DIFF-1` | Phase-Diffusion Unit | Random phase uncertainty | $\delta\phi\sim\mathcal N(0,\sigma_\phi^2)$ | $\kappa=e^{-2\sigma_\phi^2}$ |
| `MEAS-1` | Homodyne Unit | Field readout | $X_\theta=X\cos\theta+P\sin\theta$ | declared output distribution |
| `EST-1` | Estimator Unit | Infer device parameters | likelihood/covariance estimator | confidence interval |
| `CTRL-1` | Adaptive Control Unit | Select $r,\phi$ | $\pi(\hat\eta,\hat\sigma_\phi,\theta)$ | action logged; no target leakage |
| `CERT-1` | Evidence Certificate Unit | Reproducibility | hashes/calibration/raw IDs | provenance complete |

The units have distinct roles. A vacuum state is not a particle, a loss channel is not a state, a controller is not a quantum mode, and a test result is not a physical measurement.

### Legal composition

A sensor run composes as

$$
\mathrm{VAC\text{-}0}\circ\mathrm{MOD\text{-}1}\circ\mathrm{SQ\text{-}1}\circ\mathrm{PHI\text{-}1}\circ\mathrm{LOSS\text{-}1}\circ\mathrm{DIFF\text{-}1}\circ\mathrm{MEAS\text{-}1}.
$$

A closed-loop run adds estimation, control, and provenance:

$$
\mathrm{SensorRun}\circ\mathrm{EST\text{-}1}\circ\mathrm{CTRL\text{-}1}\circ\mathrm{CERT\text{-}1}.
$$

---

## 4. Exact mathematical model

### 4.1 Vacuum reference

For a single bosonic mode,

$$
a|0\rangle=0,
\qquad
\hat n=a^\dagger a,
\qquad
\hat H=\hbar\omega\left(\hat n+\frac12\right).
$$

Define dimensionless quadratures

$$
X=\frac{a+a^\dagger}{\sqrt2},
\qquad
P=\frac{a-a^\dagger}{i\sqrt2}.
$$

The vacuum reference satisfies

$$
\langle X\rangle=\langle P\rangle=0,
\qquad
V_X=V_P=\frac12,
\qquad
V_XV_P=\frac14.
$$

### 4.2 Squeezed vacuum

For squeezing strength $r\ge0$, the aligned covariance is

$$
V_s(r)=\frac12
\begin{pmatrix}
 e^{-2r}&0\\
 0&e^{2r}
\end{pmatrix}.
$$

The squeezed quadrature has variance $e^{-2r}/2$ and the conjugate quadrature has variance $e^{2r}/2$.

### 4.3 Optical loss

A transmissivity-$\eta$ loss channel mixes the mode with vacuum:

$$
V_\eta=\eta V_s+(1-\eta)\frac{I}{2},
\qquad 0\le \eta\le1.
$$

### 4.4 Gaussian phase diffusion

Let the relative phase error be $\delta\phi\sim\mathcal N(0,\sigma_\phi^2)$. The anisotropic covariance contribution is attenuated by

$$
\kappa=\mathbb E[\cos(2\delta\phi)]=e^{-2\sigma_\phi^2}.
$$

The minimum measured quadrature variance becomes

$$
V_{\min}(r;\eta,\sigma_\phi)=
\frac{1-\eta}{2}
+\frac{\eta}{2}
\left[\cosh(2r)-\kappa\sinh(2r)\right].
$$

### 4.5 Derivation of the control law

Differentiate with respect to $r$:

$$
\frac{\partial V_{\min}}{\partial r}
=\eta\left[\sinh(2r)-\kappa\cosh(2r)\right].
$$

For $\eta>0$, stationarity requires

$$
\tanh(2r_\star)=\kappa.
$$

Therefore, for $\sigma_\phi^2>0$,

$$
 r_\star=\frac12\operatorname{artanh}(\kappa)
=\frac12\operatorname{artanh}\left(e^{-2\sigma_\phi^2}\right).
$$

Using $\cosh(2r_\star)=1/\sqrt{1-\kappa^2}$ and $\sinh(2r_\star)=\kappa/\sqrt{1-\kappa^2}$,

$$
V_{\min,\star}
=\frac{1-\eta}{2}
+\frac{\eta}{2}\sqrt{1-\kappa^2}
=\frac{1-\eta}{2}
+\frac{\eta}{2}\sqrt{1-e^{-4\sigma_\phi^2}}.
$$

The second derivative at the stationary point is

$$
\frac{\partial^2V_{\min}}{\partial r^2}
=2\eta\left[\cosh(2r)-\kappa\sinh(2r)\right]
=2\eta\sqrt{1-\kappa^2}>0,
$$

so the stationary point is a strict minimum whenever $\eta>0$ and $\sigma_\phi^2>0$.

For zero phase diffusion, $\kappa=1$ and there is no finite optimum in the ideal loss-only model; the engineering controller must then impose a maximum allowable $r$ from source power, bandwidth, detector dynamic range, or model validity. This edge case is explicitly handled rather than hidden.

---

## 5. Executable implementation

### Core files

| File | Purpose |
|---|---|
| `cpp_engine/include/qau/vacuum_mode.hpp` | Fock-basis vacuum, coherent, and squeezed mode implementation |
| `cpp_engine/include/qau/vacuum_adaptive.hpp` | Analytic loss/phase-diffusion controller and covariance model |
| `cpp_engine/tests/test_vacuum_mode.cpp` | Vacuum, coherent, squeezed, phase, loss, and safety tests |
| `cpp_engine/tests/test_vacuum_adaptive.cpp` | Derivation, stationarity, local-minimum, monotonicity, and limit tests |
| `cpp_engine/tests/vacuum_mode_application.py` | Python integration, CSV export, and sensing plot |
| `cpp_engine/tests/vacuum_mode_sensing.png` | Generated diagnostic plot |
| `cpp_engine/tests/vacuum_mode_sensing_scan.csv` | Raw angle/variance sweep |
| `cpp_engine/tests/vacuum_mode_plot_review.md` | Visual inspection record |
| `qau_vacuum_units_spec.md` | Unit registry and composition contract |

### Reproduction commands

From the repository root:

```bash
cd cpp_engine/build
cmake ..
cmake --build . --parallel 2
ctest --output-on-failure
```

The adaptive derivation target can be run directly:

```bash
./vacuum_adaptive_tests
```

The result is deterministic and does not depend on pseudo-random sampling. The physical experiment described later is required before making a physical-performance claim.

### Current C++ evidence

The derivation harness was executed with:

$$
\eta=0.86,\qquad\sigma_\phi^2=0.08,
$$

which produced

$$
\kappa=0.852144,\qquad r_\star=0.631965,
\qquad V_{\min,\star}=0.295022.
$$

The harness passed five independent gates:

1. closed-form policy matches the derived expression;
2. finite-difference derivative is zero at the policy point;
3. the policy is a local minimum;
4. greater loss worsens the predicted minimum variance;
5. zero-phase and invalid-input limits behave correctly.

The previously existing vacuum-mode harness and all existing project tests also pass. These results prove software/model consistency only.

---

## 6. What exactly is solved

### Solved at the mathematical/model level

For the declared single-mode Gaussian model, the optimal squeezing strength under simultaneous transmissivity $\eta$ and Gaussian phase diffusion $\sigma_\phi^2$ is derived in closed form and implemented. The result is stronger than a smoke test because the C++ harness checks the derivative condition, local minimality, boundary case, and loss monotonicity.

### Not solved yet

The following are not established by this repository:

| Claim | Status |
|---|---|
| Vacuum is the ultimate ontological substance of nature | Not established |
| QAU is a physical quantum unit | Not established |
| NCB Saturation solves measurement collapse | Retracted/unproven |
| QAU produces squeezed light | Not established; no hardware |
| QAU improves a real detector | Not established; no physical data |
| The policy is novel relative to all literature | Not established; requires systematic literature review |
| The policy survives non-Gaussian noise and drifting devices | Not established |
| The policy beats a real fixed-squeezing controller | Not established |

This separation is part of the evidence, not a weakness. A developer must not promote the model-level result to a physical result without the next gate.

---

## 7. Real-world validation protocol

### Required equipment or dataset

A real validation requires a squeezed-light source, balanced homodyne detector, calibrated optical attenuators, phase actuator/local oscillator, acquisition system, and calibration documentation. If a public measured dataset is used instead, it must include raw or sufficiently raw quadrature traces plus vacuum reference, squeezing settings, loss/efficiency metadata, and phase information.

### Protocol

| Step | Action | Stored evidence |
|---|---|---|
| 1 | Measure shot-noise vacuum reference | raw vacuum traces, bandwidth, detector calibration |
| 2 | Generate squeezed vacuum at several $r$ values | pump/source settings and timestamps |
| 3 | Sweep homodyne phase | raw variance versus phase |
| 4 | Independently calibrate transmissivity | optical power and detector efficiency measurements |
| 5 | Estimate phase-diffusion variance | phase monitor trace and confidence interval |
| 6 | Run QAU controller on training interval | source hash, estimated parameters, chosen action |
| 7 | Evaluate on held-out interval | raw traces, variance, confidence interval |
| 8 | Repeat across drift and loss conditions | full run registry |

### Baselines

The adaptive controller must be compared with:

1. vacuum input;
2. a fixed squeezing value selected before the run;
3. a fixed phase-only controller;
4. an independently implemented optimizer;
5. an oracle controller allowed to use known calibration parameters but not held-out outcomes.

### Physical pass gate

Approval requires all of the following:

- the measured vacuum variance matches calibration within its uncertainty;
- the measured squeezed/anti-squeezed ellipse agrees with the covariance model;
- the measured loss dependence agrees with the loss channel;
- the QAU-selected $r_\star$ reduces held-out variance relative to fixed squeezing under the same resource budget;
- the improvement survives bootstrap confidence intervals and repeated runs;
- the controller does not use target data to select its parameters;
- raw data, calibration records, source hash, executable hash, and analysis code are archived;
- an independent developer reproduces the result from the archive.

If any gate fails, the claim is rejected or narrowed. Passing the C++ harness cannot substitute for this gate.

---

## 8. Independent audit checklist

An independent agent should ask:

| Audit question | Expected answer |
|---|---|
| Are the units distinct? | Yes: vacuum, mode, excitation, quadrature, phase, squeeze, loss, diffusion, measurement, estimator, controller, certificate. |
| Is the vacuum reference explicit? | Yes: $a|0\rangle=0$, $V_X=V_P=1/2$. |
| Is there a derived applied result? | Yes: finite optimal $r_\star$ under phase diffusion and loss. |
| Is the result implemented in C++? | Yes: `vacuum_adaptive.hpp`. |
| Is the derivative tested? | Yes: finite-difference stationarity check. |
| Is real physical data included? | No. This is the remaining approval gate. |
| Is a new law of physics claimed? | No. The proposal explicitly rejects that claim. |
| Can another developer reproduce the model result? | Yes, using the supplied CMake/CTest commands. |
| Can another developer claim a physical sensor improvement now? | No; hardware/data validation is required. |

---

## 9. Verdict for approval

The proposal is ready for approval as a **research-grade C++ model and controller specification**. It is not ready for approval as a demonstrated new physical quantum solution until physical sensor data or an authorized measured dataset is supplied.

The exact claim that survives audit is:

> **QAU defines a vacuum-grounded family of typed computational units and implements a mathematically derived controller for the optimal squeezing strength of a single-mode sensor under declared loss and Gaussian phase diffusion. The controller is proven for its stated model by analytic derivation and deterministic C++ tests. Its real-world quantum advantage remains an empirical question with a complete, reproducible validation protocol.**

That is the strongest statement currently supported by evidence. Any stronger statement would be an unsupported claim.

## References

[1] LIGO Caltech, “LIGO Surpasses the Quantum Limit.” [https://www.ligo.caltech.edu/news/ligo20231023](https://www.ligo.caltech.edu/news/ligo20231023)

[2] Frascella, G. et al., “Overcoming detection loss and noise in squeezing-based optical sensing,” *npj Quantum Information* 7, 72 (2021). [https://www.nature.com/articles/s41534-021-00407-0](https://www.nature.com/articles/s41534-021-00407-0)

[3] Jia, W. et al., “Squeezing the quantum noise of a gravitational-wave detector below the standard quantum limit,” *Science* (2024). [https://www.science.org/doi/10.1126/science.ado8069](https://www.science.org/doi/10.1126/science.ado8069)

[4] Gardiner, C. W. and Zoller, P., *Quantum Noise*. Springer. [https://doi.org/10.1007/978-3-662-04103-2](https://doi.org/10.1007/978-3-662-04103-2)

[5] Walls, D. F. and Milburn, G. J., *Quantum Optics*. Springer. [https://doi.org/10.1007/978-3-662-03193-4](https://doi.org/10.1007/978-3-662-03193-4)
