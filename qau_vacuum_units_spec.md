# QAU Vacuum Unit Specification

## Purpose

This specification defines a family of **Quantum Absolute Units** for a single bosonic field mode and a phase-sensitive quantum-sensing controller. The word “absolute” refers to a fixed operational reference and invariant contract inside the QAU engine; it does not assert that software has discovered the ontological bottom of nature.

The unit family is deliberately decomposed. A vacuum field mode, a phase, a loss channel, a squeezing resource, and a measurement are different physical roles and must not be collapsed into one vague object.

## Unit registry

| ID | Unit name | Role | Mathematical object | Required invariant |
|---|---|---|---|---|
| `VAC-0` | Vacuum Reference Unit | Ground-state baseline of one mode | $|0\rangle$, $a|0\rangle=0$ | $\langle n\rangle=0$, $V_X=V_P=1/2$ |
| `MOD-1` | Bosonic Mode Unit | One truncated field mode | $\mathcal{H}_N=\mathrm{span}\{|0\rangle,\ldots,|N-1\rangle\}$ | $\operatorname{Tr}\rho=1$, cutoff declared |
| `EXC-1` | Excitation Unit | Occupation above vacuum | $n=a^\dagger a$ | $n\ge0$, energy $E=\hbar\omega(n+1/2)$ |
| `QX-1` | X-Quadrature Unit | Signal/noise coordinate | $X=(a+a^\dagger)/\sqrt2$ | variance participates in uncertainty bound |
| `QP-1` | P-Quadrature Unit | Conjugate coordinate | $P=(a-a^\dagger)/(i\sqrt2)$ | $V_XV_P\ge1/4$ |
| `PHI-1` | Phase Reference Unit | Local-oscillator / squeeze orientation | $R(\phi)$ | rotation preserves spectrum and energy |
| `SQ-1` | Squeeze Resource Unit | Noise redistribution resource | $S(r,\phi)$ | $V_XV_P\ge1/4$, anti-squeezed conjugate |
| `LOSS-1` | Loss Channel Unit | Coupling to vacuum port | $\rho\mapsto\mathcal{L}_\eta(\rho)$ | completely positive, trace preserving; $V\mapsto\eta V+(1-\eta)I/2$ |
| `DIFF-1` | Phase-Diffusion Unit | Random phase uncertainty | $\delta\phi\sim\mathcal N(0,\sigma_\phi^2)$ | covariance positive and $\kappa=e^{-2\sigma_\phi^2}\in(0,1]$ |
| `MEAS-1` | Homodyne Measurement Unit | Observable readout | $X_\theta=X\cos\theta+P\sin\theta$ | output distribution has declared mean/variance |
| `EST-1` | Estimator Unit | Infer $\eta,\sigma_\phi,r,\phi$ from data | likelihood / covariance estimator | confidence interval and held-out score |
| `CTRL-1` | Adaptive Control Unit | Select $r,\phi$ using estimated conditions | policy $\pi(\hat\eta,\hat\sigma_\phi,\theta)$ | cannot use held-out target data; action logged |
| `CERT-1` | Evidence Certificate Unit | Make results auditable | hashes, calibration, raw trace IDs | provenance complete and reproducible |

## Composition

A valid sensor configuration is a typed composition:

$$
\mathrm{SensorRun}=\mathrm{VAC\text{-}0}\circ\mathrm{MOD\text{-}1}\circ\mathrm{SQ\text{-}1}\circ\mathrm{PHI\text{-}1}\circ\mathrm{LOSS\text{-}1}\circ\mathrm{DIFF\text{-}1}\circ\mathrm{MEAS\text{-}1}.
$$

For a control run, add estimation and control:

$$
\mathrm{ClosedLoopRun}=\mathrm{SensorRun}\circ\mathrm{EST\text{-}1}\circ\mathrm{CTRL\text{-}1}\circ\mathrm{CERT\text{-}1}.
$$

Composition is legal only when dimensions and units match. A loss channel acts on a mode; a phase acts on a mode or quadrature frame; an estimator consumes data; a controller consumes estimates and emits settings. A controller is not itself a quantum state.

## Core covariance mathematics

For aligned squeezed vacuum,

$$
V_s(r)=\frac12\begin{pmatrix}e^{-2r}&0\\0&e^{2r}\end{pmatrix}.
$$

For phase orientation $\phi$, define

$$
R(\phi)=\begin{pmatrix}\cos\phi&-\sin\phi\\\sin\phi&\cos\phi\end{pmatrix},
\qquad V_\phi=R(\phi)V_sR(\phi)^T.
$$

Loss mixes the mode with vacuum:

$$
V_\eta=\eta V_\phi+(1-\eta)\frac{I}{2}.
$$

If phase diffusion is Gaussian with variance $\sigma_\phi^2$, its anisotropic component is attenuated by

$$
\kappa=e^{-2\sigma_\phi^2}.
$$

The minimum quadrature variance for an aligned measurement is

$$
V_{\min}(r;\eta,\sigma_\phi)=\frac{1-\eta}{2}+\frac{\eta}{2}\left[\cosh(2r)-\kappa\sinh(2r)\right].
$$

Differentiating with respect to $r$ gives the finite optimum for $0<\kappa<1$:

$$
\tanh(2r_\star)=\kappa,
\qquad r_\star=\frac12\operatorname{artanh}(\kappa).
$$

The predicted optimum is

$$
V_{\min,\star}=\frac{1-\eta}{2}+\frac{\eta}{2}\sqrt{1-\kappa^2}.
$$

This is the mathematical kernel of the proposed solution: a controller can select a finite squeezing strength when phase diffusion is nonzero, rather than increasing squeezing blindly toward a loss floor.

## Unit maturity gates

A unit is “mature” only when it has all of the following:

1. A typed mathematical definition.
2. A legal-composition rule.
3. Invariants and domain checks.
4. An independent reference calculation.
5. A C++ implementation.
6. Error propagation.
7. A real measurement interface or an explicit statement that no physical data have yet been connected.
8. A reproducible evidence certificate.

`VAC-0` through `LOSS-1` satisfy the model-level requirements in the current repository. `EST-1`, `CTRL-1`, and `CERT-1` are the research-development target and are not yet proven against physical data.

## Forbidden conflations

The vacuum reference is not a particle. A squeezed state is not the vacuum itself. A C++ Fock vector is not a physical field. A passing unit test is not a physical experiment. A predicted sensitivity improvement is not a measured sensitivity improvement. The specification preserves these boundaries so another developer can extend the system without silently turning a model into a claim.
