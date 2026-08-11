# QAU Ultimate Real-Quantum-Engine Roadmap

## Claim Boundary First

This proposal accepts the project’s intended identity as a **quantum-computing engine**, but it makes the execution boundary exact:

> A program running on a personal computer is a real **quantum compiler, runtime, scheduler, backend adapter, and verification system** only when it produces a hardware-executable payload, submits it to a physical quantum-processing unit, receives hardware measurement data, and preserves an auditable execution receipt.

The personal computer is the host and control plane. The quantum processor is the execution plane. If no physical QPU is connected or reached through an authorized service, the result is not a quantum-computer execution result. It is either a compile result, a static-analysis result, or an emulator/oracle result. The emulator may remain in the repository for development and differential testing, but it must never be presented as the engine’s physical execution path.

OpenQASM 3 is explicitly designed as an intermediate representation for higher-level compilers to communicate with quantum hardware, and it includes classical feed-forward, timing intent, and pulse-level definitions [1]. QIR provides a hardware-agnostic LLVM-based interface between quantum programming languages/frameworks and target platforms [2]. A production architecture therefore needs more than a state-vector class: a compiler, target model, executable payload, runtime service, hardware drivers, result transport, and diagnostics. The IBM Quantum Engine Compiler architecture describes this separation directly [3].

## The New QAU Product Definition

QAU should evolve from a collection of quantum-themed primitives into a **hardware-facing quantum execution platform** with four explicitly separated products.

| Product | Purpose | Physical status |
|---|---|---|
| `qau-compiler` | Parse QAU language/OpenQASM 3, verify programs, lower to target-native instructions, and emit payloads | Runs on the user’s PC; no simulation required |
| `qau-runtime` | Submit, schedule, monitor, cancel, retrieve, authenticate, and audit hardware jobs | Runs on the PC or server and talks to a real QPU service/controller |
| `qau-backends` | Adapters for vendor/cloud QPUs and eventually laboratory controllers | Executes on physical quantum hardware when credentials and device access exist |
| `qau-oracle` | Optional small-register emulator and mathematical reference used only for compiler tests and differential validation | Not part of the physical execution claim |

The public product name should be **QAU Quantum Engine** only after the first reproducible hardware job passes the acceptance gates below. Until then, the honest status is **QAU Hardware-Ready Compiler/Runtime Prototype**.

## End-to-End Physical Execution Contract

Every non-simulated QAU execution must follow this path:

```text
QAU source / OpenQASM 3 / QIR
        ↓
Lexer + parser + typed semantic verifier
        ↓
Target-independent Quantum IR
        ↓
Target capability check and resource report
        ↓
Layout, routing, gate decomposition, timing, and dynamic-control lowering
        ↓
Target payload / OpenQASM 3 / QIR / pulse or controller instruction stream
        ↓
Backend adapter with authenticated device identity
        ↓
Physical QPU execution
        ↓
Raw shots, metadata, calibration snapshot, timestamps, and job ID
        ↓
Result decoder + provenance ledger + verification analysis
        ↓
Signed execution receipt and claim verdict
```

The output of a QAU hardware run must contain the backend name, device identifier, device calibration version or timestamp, compiler commit, source hash, IR hash, payload hash, qubit mapping, shot count, queue and execution timestamps, raw measurement data, mitigation settings, and statistical confidence intervals. A result without this provenance is not sufficient evidence for a physical-execution claim.

## Claim Taxonomy and Verdict Rules

The audit must stop using one undifferentiated “green” label. Each claim receives a status with evidence requirements.

| Verdict | Meaning | Minimum evidence |
|---|---|---|
| **PROVEN** | The exact claim is reproduced independently under stated conditions | Source, test, artifact, environment, and reproducible result |
| **HARDWARE-VERIFIED** | A physical QPU executed the workload | Device receipt, raw shots, metadata, rerun, and statistical analysis |
| **ENGINE-READY** | Compiler/runtime path is implemented but no physical run is yet evidenced | Payload, backend contract tests, mocked transport, and static validation |
| **PARTIAL** | A restricted or toy subset exists | Scope statement, passing tests, and explicit unsupported cases |
| **ORACLE-ONLY** | Only emulator/reference execution is available | Independent oracle agreement; no hardware claim allowed |
| **UNSUPPORTED** | No implementation or evidence supports the claim | Remove from capability list or mark roadmap-only |
| **REJECTED** | The claim contradicts the implementation or physical boundary | Rename, remove, or redesign before further work |

A claim cannot become **HARDWARE-VERIFIED** through a local unit test. A claim cannot become **SCALABLE** through one benchmark point. A claim cannot become **PHYSICAL QUANTUM** through a classical amplitude vector.

## Baseline Verdict of the Existing Repository

| Existing claim | Immediate verdict | Required replacement |
|---|---|---|
| “Quantum Virtual Substrate executes quantum primordials natively on silicon” | **REJECTED as a physical claim** | Classical host-side compiler/runtime primitives; physical execution requires a QPU backend |
| ASC/RPW/NCB are irreducible quantum primitives | **UNSUPPORTED as physics**; **PARTIAL as abstraction** | QAU intermediate abstractions for amplitude, phase, correlation, and constraints |
| C++ engine performs quantum computation | **ORACLE-ONLY** today | Hardware execution after a verified backend submits real jobs |
| MPS scales polynomially | **REJECTED at current implementation** | Implement real bond tensors, SVD, truncation, and benchmarks or remove the claim |
| Tensor contraction path is optimal | **REJECTED** | Cost-aware search with independent reference comparison |
| Phase III is hardware acceleration | **PARTIAL** | OpenMP CPU optimization; no GPU claim until a real GPU backend exists |
| Shor/QEC/QFT/QML/QKD layers are production algorithms | **REJECTED** | Rename demonstrations or implement complete algorithms with domain tests |
| Page curve is black-hole evaporation | **REJECTED as physical claim** | Closed-system subsystem-entropy experiment only |
| Phase IV produces emergent Einstein dynamics | **REJECTED** | Discrete graph-geometry response model until action, constraints, and convergence exist |
| Phase V synthesizes a wormhole | **REJECTED** | ER=EPR-inspired teleportation protocol until a defined dual model and geometry are demonstrated |

## Ultimate Roadmap

### Gate 0 — Claim Reset and Repository Integrity

The first gate is not new physics. It is semantic correctness. Rewrite the README, API documentation, roadmap, filenames, namespaces, and benchmark labels so that `qau-oracle`, `qau-emulator`, `qau-toy-models`, and `qau-hardware` are impossible to confuse. Remove claims that the current state-vector code is physical quantum execution. Move generated build directories out of version control, add a clean `.gitignore`, introduce reproducible dependency discovery, and create a capability manifest generated from the build.

**Exit evidence:** a claim-to-test registry, a clean repository, a clean build from a fresh checkout, and zero public claims that exceed the current evidence.

### Gate 1 — Hardware-Neutral Core IR

Replace the current ad hoc instruction structures with a typed, versioned Quantum Intermediate Representation. The IR must represent qubits, bits, parameters, measurements, resets, barriers, timing, classical conditions, loops with bounded execution semantics, calibration references, and explicit resource requirements. It must have a stable serialization format and schema version.

QAU should support OpenQASM 3 import/export and should evaluate QIR as a second interoperability target. OpenQASM 3’s role as a hardware-facing IR and QIR’s LLVM-based interoperability make them more credible integration boundaries than a closed QAU bytecode alone [1] [2].

**Exit evidence:** parser conformance tests, round-trip tests, type-error diagnostics, malformed-program rejection, deterministic IR hashes, and cross-translation tests for OpenQASM 3 and QAU IR.

### Gate 2 — Compiler Correctness Before Hardware

Build a real compiler pipeline:

1. Parse source into an AST.
2. Perform type, lifetime, measurement, classical-control, and resource checks.
3. Lower AST to target-independent QAU IR.
4. Verify gate legality, unitary annotations, measurement semantics, and qubit aliasing.
5. Optimize only through semantics-preserving passes with before/after proof obligations.
6. Map logical qubits to target qubits.
7. Route interactions through the target connectivity graph.
8. Decompose abstract gates into the target basis.
9. Schedule durations, barriers, measurement, reset, and feedback.
10. Emit a target payload and a machine-readable compilation report.

The compiler report must show gate counts before and after optimization, depth, two-qubit count, routing overhead, estimated duration, active qubits, measurement count, and unsupported features. “Optimized” means the report provides a measured reduction while preserving behavior under independent verification.

**Exit evidence:** differential tests against an independent reference, randomized circuits, property-based equivalence tests, and target-specific compilation snapshots.

### Gate 3 — Real Backend Adapter Contract

Create a backend interface with explicit capabilities rather than vendor-specific shortcuts:

```text
Backend::describe_device()
Backend::compile_or_accept_payload()
Backend::submit(payload, options)
Backend::status(job_id)
Backend::cancel(job_id)
Backend::fetch_results(job_id)
Backend::fetch_calibration_snapshot()
Backend::close()
```

The device description must include qubit count, connectivity, native gates, parameter ranges, measurement/reset support, timing granularity, dynamic-circuit support, calibration timestamp, queue state, and authentication requirements. The adapter must distinguish compilation failure, queue failure, device failure, transport failure, partial result, and successful hardware execution.

Implement one real remote-QPU adapter first, then a second independent adapter. Do not claim portability after a single vendor. A mock backend may test runtime mechanics, but it cannot count as hardware verification.

**Exit evidence:** authenticated integration test on a physical device, raw job receipt, device metadata, re-fetchable results, retry/idempotency behavior, and failure-injection tests.

### Gate 4 — First Physical Bell Pair and Measurement Receipt

The first hardware milestone should be deliberately small and auditable:

1. Prepare $|00\rangle$.
2. Apply $H$ and CNOT.
3. Measure both qubits for a statistically sufficient shot count.
4. Record raw counts and calibration metadata.
5. Compute parity, marginal distributions, confidence intervals, and total-variation distance from the ideal distribution.
6. Repeat at multiple times and on at least two devices or calibration windows.

This is the first moment at which QAU can truthfully say: **“QAU submitted and verified a workload on physical quantum hardware.”** The claim is about the execution path, not about quantum advantage.

**Exit evidence:** reproducible Bell-pair hardware receipt, raw counts, statistical report, source/payload hashes, device identity, and an independent rerun.

### Gate 5 — Real Hardware Verification and Calibration Layer

Implement QAU’s quantum characterization, verification, and validation layer. It must include measurement assignment characterization, single-qubit randomized benchmarking, two-qubit randomized benchmarking, interleaved benchmarking where justified, leakage checks, drift monitoring, calibration snapshots, and workload-level fidelity metrics. Randomized benchmarking is appropriate because it estimates computationally relevant gate errors while reducing dependence on perfect state preparation and measurement and tests error behavior over long sequences [4].

No QAU optimization may be called an improvement until it is measured on the same hardware under comparable calibration conditions. Every performance claim must report shot count, confidence interval, device, date, queue latency, execution latency, compilation latency, and whether mitigation was used.

**Exit evidence:** characterization dashboard, raw experiment archive, uncertainty estimates, regression alerts, and a device-specific capability profile consumed by the compiler.

### Gate 6 — Runtime, Dynamic Circuits, and Hybrid Control

Upgrade `qau-runtime` from job submission to a real execution control plane. It must support asynchronous jobs, cancellation, retries with idempotency, shot batching, parameter binding, measurement-conditioned control, mid-circuit measurement if the device supports it, reset, result streaming where available, and secure credential management.

This is where QAU becomes more than a circuit compiler. The runtime must coordinate host classical computation, hardware feedback, calibration state, and result provenance. It must respect device timing and cannot silently move a runtime operation into a host-side simulation.

**Exit evidence:** a real dynamic-circuit workload with hardware-side classical feedback, timing report, device capability proof, and an execution receipt that identifies which operations executed on-device.

### Gate 7 — QAU Research Workloads on Hardware

Only after Gates 4–6 should the existing research modules be reintroduced as hardware workloads:

| Module | Hardware-ready claim |
|---|---|
| OTOC | Execute small operator-growth circuits on a real QPU, report finite-size and noise effects, and compare hardware data with an independent oracle |
| Entropy | Use randomized measurements, shadows, or tomography protocols with uncertainty bars; do not infer exact entropy from one noisy shot set |
| Page-like curves | Rename as subsystem-entropy experiments unless a field-theoretic model and radiation process are implemented |
| Graph geometry | Treat as a classical compiler/runtime research analysis unless a quantum encoding and measurable observable are specified |
| ER=EPR-inspired protocol | Execute teleportation-style circuits on hardware and report transfer fidelity, correction latency, and causal controls; do not call it a wormhole |
| QML | Implement parameter binding, objective evaluation, gradients, optimizer, shot noise, and hardware training curves |

**Exit evidence:** each module has a hardware execution receipt, independent oracle comparison, finite-shot uncertainty, device-noise analysis, and a claim-specific verdict.

### Gate 8 — Real MPS/Tensor-Network Subsystem or Claim Removal

The MPS layer is a separate engineering track. It should either be completed as a genuine tensor-network implementation or removed from the advertised engine. A real implementation needs three-index site tensors, nontrivial bond dimensions, canonical forms, two-site gate updates, SVD truncation with error accounting, observables, sampling, and dense cross-checks. The contraction optimizer needs an actual cost model, dimension-aware path search, and comparison against established planners.

This subsystem is useful for compiler optimization and oracle generation, but it remains classical infrastructure. It must never be marketed as quantum hardware execution.

**Exit evidence:** dense-versus-MPS agreement, truncation-error curves, memory/time/error scaling, cost-model validation, and reproducible benchmarks on multiple systems.

### Gate 9 — Error Mitigation, Error Correction, and Fault-Tolerant Readiness

QAU should separate three concepts:

| Concept | Meaning |
|---|---|
| Error characterization | Measure hardware noise and drift |
| Error mitigation | Estimate improved observables without creating logical qubits |
| Error correction | Encode logical information and actively detect/correct errors |

The current Bell-bond demonstration is not QEC. A real QEC path requires stabilizer representation, syndrome extraction, decoder integration, logical state preparation, logical measurement, fault models, and logical error-rate evidence. A fault-tolerant claim requires repeated logical cycles and a demonstrated logical error rate below the relevant physical baseline, not merely an encoded circuit that runs.

**Exit evidence:** a named code, decoder, syndrome dataset, logical error curves, and a reproducible hardware experiment.

### Gate 10 — Production Engineering and Security

A real engine needs production discipline: semantic versioning, stable APIs, signed releases, reproducible builds, CI across supported platforms, sanitizers, fuzzing, deterministic compiler tests, secrets isolation, audit logs, retry safety, backend access control, quota handling, and data retention policy. Hardware credentials must never be stored in source, test logs, or committed configuration.

The build must replace hard-coded Eigen paths, use target-based CMake, separate safe numerical flags from benchmark flags, and stop globally enabling `-ffast-math` for correctness-critical code. `-march=native` belongs only in an explicitly labeled local benchmark configuration.

**Exit evidence:** release artifacts from clean builders, CI matrix, vulnerability scan, fuzzing report, signed provenance receipts, and documented incident/failure handling.

## End-to-End Audit Protocol

Every future claim must be tested through the following audit packet:

| Audit artifact | Required content |
|---|---|
| Claim ID | Stable identifier such as `QAU-HW-EXEC-001` |
| Claim text | One narrow, falsifiable sentence |
| Execution class | Compile-only, oracle-only, mocked backend, cloud QPU, or laboratory QPU |
| Source hash | Exact source and configuration identity |
| Compiler hash | QAU compiler version and commit |
| Payload hash | Exact submitted representation |
| Backend identity | Vendor, device, qubit topology, calibration timestamp |
| Raw evidence | Counts, traces, timestamps, status, and metadata |
| Reference comparison | Independent analytic/oracle result and tolerance |
| Statistics | Shots, confidence intervals, uncertainty model |
| Reproduction | Rerun instructions and independent rerun result |
| Verdict | Proven, hardware-verified, engine-ready, partial, oracle-only, unsupported, or rejected |

No claim is accepted without a claim ID and an audit packet. “All harnesses green” becomes one field in the packet rather than the final verdict.

## Ultimate Acceptance Matrix

| Milestone | Pass condition | Resulting claim |
|---|---|---|
| Clean host build | Fresh checkout builds without machine-specific paths | Reproducible QAU toolchain |
| IR conformance | OpenQASM 3/QIR round trips and rejects invalid programs | Standards-oriented compiler front end |
| Compiler equivalence | Randomized circuits match independent reference under declared semantics | Correct target-independent compiler |
| Mock runtime | Jobs, retries, metadata, and failure states behave correctly | Runtime-engine ready |
| First QPU Bell pair | Physical device receipt and raw counts verified | First hardware-executed QAU workload |
| QCVV suite | RB, measurement, drift, and workload metrics pass thresholds | Device-aware verified runtime |
| Dynamic circuit | Hardware-side measurement feedback verified | Real hybrid quantum-classical runtime |
| Two backends | Same IR executes on two physical backends with capability-aware lowering | Portable quantum engine |
| QAU research workload | Hardware run plus independent oracle plus uncertainty | Hardware-verified QAU experiment |
| Release | CI, security, provenance, packaging, and documentation gates pass | Production-grade release candidate |

## Final Verdict Categories for the Current Project

The current repository should carry this status until the physical execution gates are passed:

| Capability | Current verdict |
|---|---|
| Classical host compiler/runtime prototype | **PARTIAL / ENGINE-READY** |
| Small-register quantum computation | **ORACLE-ONLY** |
| Physical quantum execution | **NOT YET PROVEN** |
| Hardware backend integration | **NOT YET PROVEN** |
| Native quantum hardware control | **UNSUPPORTED** until a controller/QPU path exists |
| MPS scalability | **REJECTED at current implementation** |
| QFT/QML/QEC/QKD production capability | **PARTIAL demonstrations only** |
| Quantum-gravity simulation | **REJECTED**; toy-model diagnostics only |
| Wormhole synthesis | **REJECTED**; teleportation toy model only |

## The Honest Ultimate Claim

The ultimate goal is valid, but the wording must be exact:

> **QAU is a hardware-facing quantum-computing engine whose host-side compiler and runtime execute quantum programs on physical QPUs through verified backend adapters. Classical emulation is an optional oracle for development and compiler correctness; it is not the execution engine.**

That is the real-engineering version of the ambition. To earn it, QAU must first submit a reproducible physical Bell-pair circuit, receive real hardware measurements, preserve an auditable execution receipt, and then expand through compiler correctness, backend portability, runtime feedback, device benchmarking, error mitigation, and eventually logical-qubit experiments.

## References

[1] OpenQASM 3.0 Specification, “Introduction.” [https://openqasm.com/versions/3.0/intro.html](https://openqasm.com/versions/3.0/intro.html)

[2] Microsoft Azure Quantum, “Quantum Intermediate Representation.” [https://learn.microsoft.com/en-us/azure/quantum/concepts-qir](https://learn.microsoft.com/en-us/azure/quantum/concepts-qir)

[3] Healy, M. B. et al., “Design and architecture of the IBM Quantum Engine Compiler,” arXiv:2408.06469. [https://arxiv.org/html/2408.06469v1](https://arxiv.org/html/2408.06469v1)

[4] Knill, E. et al., “Randomized Benchmarking of Quantum Gates,” NIST / *Physical Review A* (2007). [https://www.nist.gov/publications/randomized-benchmarking-quantum-gates](https://www.nist.gov/publications/randomized-benchmarking-quantum-gates)
