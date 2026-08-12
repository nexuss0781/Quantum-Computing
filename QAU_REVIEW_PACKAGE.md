# QAU Independent Review Package: Real Measured Application

## Approval question

Can an independent agent run a real quantum-powered calibration application from this package, inspect measured evidence, and extend it toward laboratory control without mistaking the QAU unit registry for an unsupported claim of new physics?

## The application that must be reviewed

This package implements **measured pump-phase calibration for a squeezed-microwave TWPA readout**. The input is a public QCoDeS measurement record from the experiment of Esposito et al. [1] [2], archived at Zenodo DOI [10.5281/zenodo.5217997](https://doi.org/10.5281/zenodo.5217997) [3]. The source record contains a 25-point pump-phase sweep from 0 to pi in run IDs 11 through 35.

The operator problem is concrete: a pump phase of 0 radians is the default; measured channel 0 is evaluated at all 25 settings; the application recommends the setting with the lowest measured variance and emits a phase/run-ID setpoint for the next actuator command.

## Verified measured result

Using 100,000 measured values per setting and 2,500,000 C++ observations in total, the application produces:

| Quantity | Result |
|---|---:|
| Default phase | 0 rad, run 11 |
| Recommended phase | 2.0943951023932 rad, run 27 |
| Default variance | $5.5237370284\times10^{-7}$ |
| Recommended variance | $5.4234787984\times10^{-7}$ |
| Variance reduction | 1.8150435% |
| Reduction | 0.07955048 dB |
| Bootstrap variance-difference 95% CI | $3.06046\times10^{-9}$ to $1.67679\times10^{-8}$ |
| Full CTest suite | 13/13 passed |

This reproduces the stakeholder’s reported run-27 operating decision from real measured data. It is not simulated data, and it is not a live actuator test.

## Start here

Read `Research.md` first. Then inspect these application files:

| File | Purpose |
|---|---|
| `cpp_engine/include/qau/phase_calibrator.hpp` | Public C++ calibration API and policy types |
| `cpp_engine/src/phase_calibrator.cpp` | Welford streaming statistics, CSV ingestion, recommendation, evidence serialization |
| `cpp_engine/apps/qau_phase_calibrate.cpp` | C++ operator executable |
| `cpp_engine/apps/real_phase_calibration.py` | Python measured-array workflow, bootstrap evidence, plot, operator report |
| `cpp_engine/tests/test_phase_calibrator.cpp` | C++ application tests |
| `tools/prepare_zenodo_schema.py` | Bounded schema preparation for the 25.6 GB public SQLite source |
| `tools/download_real_phase_array.py` | Bounded measured-data extraction to NPY |
| `data/real_phase_samples_100000.npy` | Extracted real measured values used for the included result |
| `data/real_phase_samples_100000.metadata.json` | Source checksum, phase/run map, array shape, extraction metadata |
| `artifacts/real_phase_calibration_channel0_all/` | Included run-27 result, plot, summary, JSON, and report |
| `artifacts/real_phase_calibration_channel0_train/` | Training-split diagnostic with held-out odd samples |

## Reproduction commands

Build and test the C++ engine:

```bash
cd cpp_engine/build
cmake ..
cmake --build . --parallel 2
ctest --output-on-failure
```

Run the included real measured array without downloading the 25.6 GB source again:

```bash
python3 cpp_engine/apps/real_phase_calibration.py \
  --array data/real_phase_samples_100000.npy \
  --calibrator cpp_engine/build/qau_phase_calibrate \
  --output-dir artifacts/reviewer_channel0_all \
  --channel 0 \
  --default-phase-index 1 \
  --selection all \
  --bootstrap-repeats 1000
```

The expected result is run 27 at approximately 2.0943951023932 radians, with an observed variance reduction close to 1.815% and approximately 0.07955 dB.

For a leakage-resistant diagnostic, select on even samples and evaluate odd samples:

```bash
python3 cpp_engine/apps/real_phase_calibration.py \
  --array data/real_phase_samples_100000.npy \
  --calibrator cpp_engine/build/qau_phase_calibrate \
  --output-dir artifacts/reviewer_channel0_train \
  --channel 0 \
  --default-phase-index 1 \
  --selection train \
  --bootstrap-repeats 1000
```

The all-observation result is the required stakeholder reproduction. The training-split result is an additional diagnostic; neither is a live closed-loop actuator experiment.

## Source-data reproduction

The full public source is a 25.6 GB SQLite/QCoDeS object. The package deliberately avoids forcing a full download during ordinary review. To reconstruct the included NPY extraction from the source:

```bash
python3 tools/prepare_zenodo_schema.py \
  --output-dir data/zenodo \
  --schema-output data/zenodo/sqlite_master_extracted.txt

python3 tools/download_real_phase_array.py \
  --schema data/zenodo/sqlite_master_extracted.txt \
  --output data/real_phase_samples_100000.npy \
  --samples 100000
```

The extractor uses public HTTP byte ranges, records the source MD5 `59590d1cdf5fead5283493764164c9f1`, and writes metadata. No random or simulated values are used.

## Independent approval gates

| Gate | Evidence required | Status |
|---|---|---|
| Public measured source is named and hashed | DOI, URL, archive MD5 | Pass |
| 25 measured phase settings are mapped | Phase index and run IDs 11–35 | Pass |
| C++ application consumes measured values | `qau_phase_calibrate` source and CSV | Pass |
| Real recommendation is emitted | JSON setpoint and run ID | Pass |
| Stakeholder result is reproduced | Run 27, 2.0943951023932 rad | Pass |
| Measured effect is quantified | 1.815% / 0.07955 dB | Pass |
| Uncertainty is reported | Bootstrap variance-difference interval | Pass |
| Tests are green | 13/13 CTest tests | Pass |
| Pump-phase actuator is driven live | Laboratory hardware record | Pending |
| QAU is a new fundamental physical unit | Physical theory proof | Not claimed |

## QAU scope

The vacuum-grounded QAU units are engineering contracts with distinct roles: `VAC-0`, `MOD-1`, `EXC-1`, `QX-1`, `QP-1`, `PHI-1`, `SQ-1`, `LOSS-1`, `DIFF-1`, `MEAS-1`, `EST-1`, `CTRL-1`, and `CERT-1`. They define reference state, mode, quadrature, phase, measured squeezing resource, channels, estimator, controller, and provenance separately. The package does not claim that these software types replace physical quantum optics or resolve measurement collapse.

## References

[1] M. Esposito et al., “Observation of Two-Mode Squeezing in a Traveling Wave Parametric Amplifier,” *Physical Review Letters* 128, 153603 (2022), [https://doi.org/10.1103/PhysRevLett.128.153603](https://doi.org/10.1103/PhysRevLett.128.153603).

[2] M. Esposito et al., “Observation of two-mode squeezing in a traveling wave parametric amplifier,” arXiv:2111.03696, [https://arxiv.org/abs/2111.03696](https://arxiv.org/abs/2111.03696).

[3] M. Esposito, A. Ranadive, and N. Roch, “Raw data: Observation of two-mode squeezing in a traveling wave parametric amplifier,” Zenodo, [https://doi.org/10.5281/zenodo.5217997](https://doi.org/10.5281/zenodo.5217997).
