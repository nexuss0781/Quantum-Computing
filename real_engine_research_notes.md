# Real Quantum-Engine Research Notes

## OpenQASM 3

The OpenQASM 3 specification describes a measurement-based quantum programming language with classical feed-forward control. It is intended as an intermediate representation for higher-level compilers to communicate with quantum hardware. Its design goals include classical control flow, explicit timing, and embedded pulse-level definitions. The specification explicitly separates the language from the execution environment and allows hardware implementations to support only the features they can perform efficiently in real time.

Source: OpenQASM 3.0 Specification, https://openqasm.com/versions/3.0/intro.html

## Production quantum-engine architecture

The IBM Quantum Engine Compiler paper describes a production-oriented stack in which users submit OpenQASM 3, a runtime invokes a compiler, the compiler produces an executable payload, an execution service loads payloads through hardware drivers, control electronics execute the payload on a quantum device, and result data returns through the runtime. The compiler uses MLIR-based multiple intermediate representations, target-system modeling, lowering to hardware, payload generation, diagnostics, and Python interfaces. The target system includes classical control electronics, waveform generators, receivers, a hub for classical processing and feed-forward, host orchestration, and a dilution refrigerator containing qubits.

This architecture establishes the boundary for QAU: a true engine requires a compiler and runtime that produce executable hardware payloads and an adapter/driver path to a real QPU. A state-vector backend can remain a development oracle, but it cannot be the product’s execution claim.

Source: Healy et al., “Design and architecture of the IBM Quantum Engine Compiler,” arXiv:2408.06469, https://arxiv.org/html/2408.06469v1

## Audit consequence

QAU should be split into three explicit products: (1) `qau-compiler`, which parses QAU/OpenQASM and lowers to target instructions; (2) `qau-runtime`, which submits, schedules, monitors, and retrieves jobs; and (3) `qau-backends`, which contains vendor/cloud and eventually laboratory hardware adapters. Classical simulation must be renamed `qau-oracle` or `qau-emulator` and treated as a verification oracle, never as the definition of physical execution.

## QIR interoperability

Microsoft’s QIR documentation describes QIR as an intermediate representation serving as a common interface between quantum programming languages/frameworks and target platforms. QIR uses LLVM IR, supports hybrid classical and quantum logic, and represents qubits as opaque handles with quantum operations as LLVM calls. QAU should either emit OpenQASM 3, QIR, or both; inventing a closed bytecode alone would isolate the engine from existing hardware ecosystems.

Source: Microsoft Azure Quantum, “Quantum Intermediate Representation,” https://learn.microsoft.com/en-us/azure/quantum/concepts-qir

## Hardware execution boundary

The official IBM documentation page was blocked by site security in this session, so no private or gated claim is inferred from it. The public production-architecture paper remains the authoritative basis for the roadmap’s job-submission boundary: a real engine must pass compiled payloads to an execution service and hardware drivers, receive measurement results, and expose diagnostics and job status. Local emulation is a development oracle and must be reported separately from QPU execution.

## Device verification

NIST’s randomized benchmarking reference describes randomized benchmarking as a way to estimate computationally relevant gate errors without relying on accurate state preparation and measurement. Long random gate sequences also test whether errors compound and remain stable during extended computations. QAU’s real-hardware acceptance gates should therefore include single-qubit and two-qubit randomized benchmarking, calibration drift tracking, measurement assignment characterization, and workload-level performance—not only local unit tests.

Source: Knill et al., “Randomized Benchmarking of Quantum Gates,” NIST/Physical Review A, https://www.nist.gov/publications/randomized-benchmarking-quantum-gates
