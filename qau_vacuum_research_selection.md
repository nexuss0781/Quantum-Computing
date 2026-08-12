# QAU Research Selection

## Chosen open engineering problem

Design a closed-loop squeezed-vacuum sensor that remains optimal when optical transmissivity and phase diffusion vary. The problem is concrete because loss and phase noise directly reduce the quantum-noise advantage of squeezed light; it is open as an engineering-control problem because the optimum must be selected from measured, time-varying device conditions rather than from a fixed ideal state.

The proposal does not claim a new law of quantum mechanics. It derives an exact policy for a declared single-mode Gaussian model and defines the physical-data experiment needed to test whether the policy improves a real sensor.

## Model

For a squeezed vacuum with parameter r, let a=exp(-2r)/2 and b=exp(2r)/2. The covariance matrix before imperfections is

V_s = R(phi) diag(a,b) R(phi)^T.

Loss with transmissivity eta mixes the state with vacuum:

V_loss = eta V_s + (1-eta) I/2.

Phase diffusion with Gaussian phase error delta~N(0,sigma_phi^2) reduces the anisotropic covariance term by kappa=exp(-2 sigma_phi^2), because E[cos(2 delta)]=kappa. The minimum measured quadrature variance is therefore

V_min(r; eta, sigma_phi) = (1-eta)/2 + eta/2 [cosh(2r) - kappa sinh(2r)].

For fixed eta and kappa, differentiation gives

tanh(2 r_star)=kappa,

so

r_star = atanh(kappa)/2,

and

V_min_star = (1-eta)/2 + eta/2 sqrt(1-kappa^2).

The optimum phase aligns the squeezed quadrature with the sensor's signal quadrature. For kappa=1, the ideal loss-only model has no finite optimum because increasing squeezing asymptotically approaches the loss floor; for kappa<1, phase diffusion produces a finite optimum.

## Falsifiable physical application

The C++ controller will accept calibrated eta(t), sigma_phi(t), and a target signal quadrature. It will output r_star(t), the phase setting, predicted noise, and uncertainty propagation. A physical test requires a squeezed-light source, a balanced homodyne detector, independently measured loss, phase-noise estimation, and held-out detector traces.

The proposed solution is accepted only if the adaptive policy reduces held-out measured variance or increases Fisher information relative to: (a) vacuum input, (b) fixed squeezing, and (c) an independently implemented numerical optimizer. All comparisons require confidence intervals, calibration data, and raw traces.

## Evidence boundary

The equations and software can prove correctness of the declared Gaussian model. They cannot prove that the vacuum is the absolute substance of nature, nor that the policy is a new physical law, without a real optical experiment. This distinction is explicit and mandatory.
