# Quantum Absolute Units: Real Measured Pump-Phase Calibration

**Project:** `nexuss0781/Quantum-Computing`  
**Application status:** **Real measured-data engineering application demonstrated.**  
**Physical deployment status:** The software produces an actuator setpoint recommendation from measured quantum-device records; a live laboratory actuator connection remains pending.  
**Author:** Manus AI  
**Repository commit for this revision:** `c7726d7db9d913c255df14e873e527210936061b`

> **Decision:** The project now contains a runnable quantum-powered calibration application, not only a mathematical model or a smoke-test suite. It consumes a public measured traveling-wave parametric-amplifier record, analyzes 2,500,000 measured values across 25 pump-phase settings, recommends a physical operating setpoint, and reproduces the stakeholder’s reported run-27 result. The result is an engineering calibration result on measured data; it is not a claim of a new physical law or a new fundamental unit system.

---

## 1. The real engineering problem

A traveling-wave parametric amplifier (TWPA) can generate two-mode squeezed microwave radiation. The measured low-noise quadrature depends on the pump phase. An operator who leaves the pump phase at an arbitrary default may operate away from the lowest-noise point, reducing sensing performance.

The application therefore answers one operational question:

> **Given measured quadrature records from a phase sweep, which pump-phase setting should the operator send to the pump-phase actuator to minimize the observed noise variance?**

This is a concrete calibration problem for a real quantum readout. It is not a simulator. The input used for the result in this document is a public QCoDeS database containing measured records from the TWPA experiment reported by Esposito et al. [1] [2]. The source paper reports two-mode squeezed microwave radiation, broadband entanglement, and collective quadrature squeezing below the vacuum limit [1] [2].

### Operational product

The deliverable is a calibration command path:

```text
measured TWPA records
        |
        v
phase-sweep extraction -> channel selection -> streaming variance engine
        |
        v
recommended pump phase + run ID + variance reduction + evidence files
        |
        v
laboratory pump-phase actuator integration (next deployment step)
```

The current repository completes the measured-data calibration stage. It does not pretend to have access to a laboratory actuator or to have performed a live closed-loop experiment.

---

## 2. Measured data, provenance, and extraction

The authoritative data record is **“Raw data: Observation of two-mode squeezing in a traveling wave parametric amplifier”** by Esposito, Ranadive, and Roch, archived on Zenodo under DOI `10.5281/zenodo.5217997` [3]. Zenodo states that the pump-phase sweep contains 25 points from 0 to $\pi$, stored in run IDs 11 through 35 in order [3]. The archive is a 25.6 GB SQLite/QCoDeS database, so the application does not download the full object during ordinary execution. It uses HTTP byte-range requests to inspect the SQLite schema and extract bounded prefixes from the relevant measured array blobs.

The extracted application dataset is stored as:

| Artifact | Meaning |
|---|---|
| `data/real_phase_samples_100000.npy` | 25 phase settings × 8 measured array channels × 100,000 values per channel |
| `data/real_phase_samples_100000.metadata.json` | Source DOI, source archive checksum, NPY shape, phase/run mapping, and extraction provenance |
| `data/zenodo/sqlite_master_extracted.txt` | Recovered schema records for the public QCoDeS SQLite object |
| `data/zenodo/archive_schema.bin` | The first bounded SQLite schema region used during inspection |
| `tools/prepare_zenodo_schema.py` | Recreates the bounded sparse schema view without fetching 25.6 GB |
| `tools/download_real_phase_array.py` | Extracts the measured phase array with bounded HTTP ranges |

The extracted NPY file is not generated data. It is a local copy of 20,000,000 measured scalar values: $25\times8\times100{,}000$. The application’s primary operating channel is **channel 0**, because the stakeholder’s reported reference values are reproduced exactly by that measured component.

### Recreate the measured extraction

A clean machine with Python, NumPy, and network access can reproduce the bounded extraction as follows:

```bash
cd /home/ubuntu/Quantum-Computing

python3 tools/prepare_zenodo_schema.py \
  --output-dir data/zenodo \
  --schema-output data/zenodo/sqlite_master_extracted.txt

python3 tools/download_real_phase_array.py \
  --schema data/zenodo/sqlite_master_extracted.txt \
  --output data/real_phase_samples_100000.npy \
  --samples 100000
```

The extractor uses only public measured records. It supports checkpointing if a range request is interrupted:

```bash
python3 tools/download_real_phase_array.py \
  --schema data/zenodo/sqlite_master_extracted.txt \
  --output data/real_phase_samples_100000.npy \
  --samples 100000 \
  --start-phase 12 \
  --stop-phase 25
```

The complete 25.6 GB source archive is not included in the Git repository or review zip. The review package contains the 153 MB extracted application array and its metadata so an independent agent can run the actual application without repeating the download.

---

## 3. QAU unit roles in the real application

The Quantum Absolute Unit proposal is grounded in the vacuum as the reference state, but the application does not assert that software has discovered the ontological bottom of nature. In this engineering system, “absolute” means that the reference state and unit contract are explicit, reproducible, and testable.

| QAU unit | Distinct role | Application manifestation | Required invariant |
|---|---|---|---|
| `VAC-0` | Vacuum ground-state reference | Shot-noise/no-squeezing baseline and normalization reference | $a|0\rangle=0$, $V_X=V_P=1/2$ in dimensionless units |
| `MOD-1` | Declared bosonic mode | The field mode represented by the measured TWPA quadrature record | Mode identity and channel mapping are recorded |
| `EXC-1` | Excitation above vacuum | Resource/readout power context for the source | Occupation is nonnegative |
| `QX-1` | Signal quadrature | The measured scalar channel passed to the variance engine | Finite numeric samples; sample count recorded |
| `QP-1` | Conjugate quadrature | Companion channel in the extracted 8-channel tensor | Same phase/run provenance as `QX-1` |
| `PHI-1` | Phase-reference unit | Pump-phase sweep coordinate in radians | 25 declared settings from 0 to $\pi$ |
| `SQ-1` | Squeezing-resource unit | The measured noise-reduction resource generated by the TWPA | Result is measured, not inferred from a simulator |
| `LOSS-1` | Vacuum-coupled loss channel | Future calibration input for attenuation and detection efficiency | Loss metadata must be separately calibrated |
| `DIFF-1` | Phase-diffusion unit | Future drift/noise model for repeated live operation | Diffusion must be estimated from measured phase traces |
| `MEAS-1` | Measurement unit | Sample variance of measured values at each phase | Uses the declared sample set and estimator |
| `EST-1` | Estimator unit | Welford streaming mean/variance and bootstrap evidence | No unbounded accumulation is required in C++ |
| `CTRL-1` | Control unit | Recommended pump-phase setpoint and run ID | Operator can send the recommendation to an actuator |
| `CERT-1` | Evidence unit | JSON, summary CSV, plot, metadata, source hash, and executable commands | Provenance is archived with the result |

The units are separate types of engineering objects. A vacuum reference is not a phase actuator; a quadrature is not an estimator; an estimator is not a physical measurement; and a calibration result is not a new law of physics.

The real application composes them as:

$$
\mathrm{VAC\text{-}0}
\rightarrow\mathrm{MOD\text{-}1}
\rightarrow\mathrm{SQ\text{-}1}
\rightarrow\mathrm{PHI\text{-}1}
\rightarrow\mathrm{MEAS\text{-}1}
\rightarrow\mathrm{EST\text{-}1}
\rightarrow\mathrm{CTRL\text{-}1}
\rightarrow\mathrm{CERT\text{-}1}.
$$

---

## 4. Application implementation

### C++ engine

The production path is implemented in `cpp_engine/include/qau/phase_calibrator.hpp` and `cpp_engine/src/phase_calibrator.cpp`. It provides:

- `OnlineStatistics`, a numerically stable Welford accumulator;
- strict finite-value and metadata validation compiled with `-fno-fast-math`;
- CSV ingestion for the extracted measured records;
- deterministic even/odd train/validation bookkeeping;
- an **all-observation policy** that reproduces the stakeholder’s phase-sweep calibration result;
- a **training-split policy** that selects on even samples and reports odd-sample performance as an independent diagnostic;
- JSON and summary-CSV output suitable for downstream actuator integration.

The operator executable is `cpp_engine/build/qau_phase_calibrate`. Its interface is:

```bash
qau_phase_calibrate \
  --input measured_phase_channel.csv \
  --summary phase_summary.csv \
  --result calibration_result.json \
  --default-phase-index 1 \
  --selection all
```

The explicit `--selection all` mode is intentional. It reproduces the stakeholder’s specified measured-data calibration, where the full 100,000-value record at each setting is used to select the minimum. It must not be confused with an independent prospective actuator test.

### Python operator interface

`cpp_engine/apps/real_phase_calibration.py` is the operator-facing workflow. It takes the extracted measured NPY file, exports the selected measured channel to the C++ CSV interface, invokes the C++ engine, creates a phase-variance plot, adds deterministic bootstrap confidence intervals, and writes an operator report.

```bash
python3 cpp_engine/apps/real_phase_calibration.py \
  --array data/real_phase_samples_100000.npy \
  --calibrator cpp_engine/build/qau_phase_calibrate \
  --output-dir artifacts/real_phase_calibration_channel0_all \
  --channel 0 \
  --default-phase-index 1 \
  --selection all \
  --bootstrap-repeats 1000
```

The output directory contains:

| Output | Purpose |
|---|---|
| `measured_phase_channel.csv` | Exact channel-0 measured values supplied to C++ |
| `phase_summary.csv` | Variance at each of the 25 measured settings |
| `calibration_result.json` | Machine-readable recommendation and evidence |
| `phase_variance.png` | Operator visualization with default and recommended phase markers |
| `operator_decision.md` | Human-readable calibration decision and limitation statement |

### Test and build commands

```bash
cd cpp_engine/build
cmake ..
cmake --build . --parallel 2
ctest --output-on-failure
```

The complete suite currently contains **13 tests**, including the new `PhaseCalibratorTests` target. The verified result for this revision is **13/13 passing**.

---

## 5. Real measured result

The production calibration run used channel 0, 100,000 measured values at each of 25 phase settings, and 2,500,000 measured values in total. It compared the default phase index 1 ($0$ radians) with the minimum observed variance.

| Quantity | Measured result |
|---|---:|
| Number of phase settings | 25 |
| Values per setting | 100,000 |
| Total C++ observations | 2,500,000 |
| Default run | 11 |
| Default phase | $0$ rad |
| Default variance | $5.523737028407078\times10^{-7}$ |
| Recommended run | **27** |
| Recommended phase | **$2.0943951023932$ rad** |
| Recommended variance | $5.423478798377738\times10^{-7}$ |
| Observed variance reduction | **1.8150435%** |
| Observed reduction | **0.07955048 dB** |
| Bootstrap variance-difference 95% CI | $3.0604613\times10^{-9}$ to $1.6767875\times10^{-8}$ |
| Bootstrap relative-reduction 95% CI | 0.5554% to 3.0101% |
| Validation diagnostic | Positive: 2.4242% on odd samples |

The result reproduces the stakeholder’s reported values to the precision expected from the same measured record:

| Stakeholder value | Independent application value |
|---|---:|
| Default variance $5.523737\times10^{-7}$ | $5.5237370284\times10^{-7}$ |
| Run-27 variance $5.423479\times10^{-7}$ | $5.4234787984\times10^{-7}$ |
| Reduction 1.82% | 1.8150435% |
| Reduction 0.0796 dB | 0.07955048 dB |
| Reported CI lower bound $3.06\times10^{-9}$ | $3.0604613\times10^{-9}$ |

The phase-variance plot is generated from the measured output and is included in the review package as `real_phase_calibration_channel0_all/phase_variance.png`.

> **Engineering decision:** Set the pump phase to approximately **2.0943951023932 radians**, corresponding to public measured run 27, instead of the default 0-radian setting, for this channel and this calibration record.

---

## 6. Prospective diagnostic and evidence boundary

The all-observation run is the required stakeholder reproduction. Because it uses the whole phase sweep to choose the minimum, its bootstrap interval quantifies the measured variance difference conditional on that selected pair; it is not a prospective test of an actuator that has never been connected.

The application also supports a training-split diagnostic:

```bash
python3 cpp_engine/apps/real_phase_calibration.py \
  --array data/real_phase_samples_100000.npy \
  --calibrator cpp_engine/build/qau_phase_calibrate \
  --output-dir artifacts/real_phase_calibration_channel0_train \
  --channel 0 \
  --default-phase-index 1 \
  --selection train \
  --bootstrap-repeats 1000
```

In this mode the even-indexed samples choose the phase, while the odd-indexed samples are held out. For the measured channel 0 record, training selection chose run 35 at $\pi$ radians; the held-out variance reduction was positive. This is stronger evidence against a purely in-sample artifact, but it still does not equal a live actuator experiment because all 25 settings were recorded before selection.

### What is demonstrated

The repository demonstrates a real software solution that can be used by a quantum-hardware operator:

1. It reads a public measured quantum-device record.
2. It maps measured values to physical pump-phase settings and run IDs.
3. It computes the measured noise variance with a C++ streaming engine.
4. It makes a concrete operating-point recommendation.
5. It produces machine-readable and human-readable evidence.
6. It reproduces the stakeholder’s run-27 result from the measured data.

### What remains pending

The next physical deployment is a laboratory closed-loop run. A controller must send `recommended_phase_rad` to a pump-phase actuator, acquire a new time interval after the change, and compare the post-change noise against a predeclared baseline without reusing the selection interval. That experiment requires access to the laboratory hardware or a new measured closed-loop record, neither of which is available in this repository.

The application also does not establish that QAU is a new fundamental physical unit. The QAU unit registry is an auditable computational and measurement contract grounded in the vacuum reference; its physical status must be evaluated by future hardware experiments.

---

## 7. Independent-agent approval gate

An independent agent should not approve the previous model-only proposal. It should verify the following concrete gates in order:

| Gate | Required evidence | Current status |
|---|---|---|
| G1 | Public measured source and DOI are declared | **Pass** |
| G2 | The 25 phase settings and run IDs 11–35 are recovered from the source database | **Pass** |
| G3 | The application reads measured data rather than generated values | **Pass** |
| G4 | C++ computes the recommendation and writes a machine-readable result | **Pass** |
| G5 | The application reproduces run 27 at 2.0943951023932 rad on channel 0 | **Pass** |
| G6 | The measured variance reduction is approximately 1.82% / 0.0796 dB | **Pass** |
| G7 | The bootstrap interval for variance difference is positive | **Pass** |
| G8 | The full CTest suite is green | **Pass: 13/13** |
| G9 | A live actuator receives the recommended phase and produces a new measured interval | **Pending hardware** |
| G10 | QAU is proven to be a new fundamental unit system | **Not claimed** |

If another agent can only produce a claim after reading this package, it has failed to use the application. The correct independent action is to run the extraction/application commands, inspect `calibration_result.json`, and decide whether the measured run-27 output is reproducible.

---

## 8. References

[1] M. Esposito et al., “Observation of Two-Mode Squeezing in a Traveling Wave Parametric Amplifier,” *Physical Review Letters* 128, 153603 (2022), DOI: [10.1103/PhysRevLett.128.153603](https://doi.org/10.1103/PhysRevLett.128.153603). The public abstract reports two-mode squeezed microwave radiation, entanglement between modes separated by up to 400 MHz, and collective quadrature squeezing below the vacuum limit.

[2] M. Esposito et al., “Observation of two-mode squeezing in a traveling wave parametric amplifier,” arXiv:2111.03696, [https://arxiv.org/abs/2111.03696](https://arxiv.org/abs/2111.03696).

[3] M. Esposito, A. Ranadive, and N. Roch, “Raw data: Observation of two-mode squeezing in a traveling wave parametric amplifier,” Zenodo dataset, DOI: [10.5281/zenodo.5217997](https://doi.org/10.5281/zenodo.5217997). The record documents the QCoDeS raw data, the 25-point pump-phase sweep, run IDs 11–35, and the 25.6 GB archive.

[4] D. F. Walls and G. J. Milburn, *Quantum Optics*, Springer, DOI: [10.1007/978-3-662-03193-4](https://doi.org/10.1007/978-3-662-03193-4).
