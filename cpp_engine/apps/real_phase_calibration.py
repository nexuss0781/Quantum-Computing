#!/usr/bin/env python3
"""Operator workflow for the real measured squeezed-microwave phase sweep.

This script never creates measurements. It consumes a local NPY extraction of the
Zenodo records, exports one measured channel to a streaming CSV, invokes the C++
calibrator, and writes the operator-facing plot and decision report.
"""
from __future__ import annotations

import argparse
import csv
import json
import subprocess
from pathlib import Path

import matplotlib.pyplot as plt
import numpy as np


def export_channel_csv(array_path: Path, csv_path: Path, channel: int) -> int:
    measured = np.load(array_path, mmap_mode="r")
    if measured.ndim != 3 or measured.shape[0] != 25 or measured.shape[1] != 8:
        raise ValueError(f"expected array shape (25, 8, samples), got {measured.shape}")
    if not 0 <= channel < measured.shape[1]:
        raise ValueError(f"channel must be in [0, {measured.shape[1] - 1}]")
    samples = measured.shape[2]
    csv_path.parent.mkdir(parents=True, exist_ok=True)
    with csv_path.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["run_id", "phase_index", "phase_rad", "channel", "sample_index", "value"])
        for phase_index in range(1, 26):
            phase_rad = float(np.pi * (phase_index - 1) / 24.0)
            for sample_index, value in enumerate(measured[phase_index - 1, channel]):
                writer.writerow([
                    phase_index + 10,
                    phase_index,
                    f"{phase_rad:.15g}",
                    channel,
                    sample_index,
                    f"{float(value):.17g}",
                ])
    return samples


def bootstrap_variance_difference(values: np.ndarray, default_index: int, recommended_index: int, repeats: int, seed: int) -> dict[str, object]:
    default = np.asarray(values[default_index], dtype=np.float64)
    recommended = np.asarray(values[recommended_index], dtype=np.float64)
    rng = np.random.default_rng(seed)
    differences: list[np.ndarray] = []
    relatives: list[np.ndarray] = []
    batch_size = 64
    remaining = repeats
    while remaining:
        batch = min(batch_size, remaining)
        default_indices = rng.integers(0, default.size, size=(batch, default.size))
        recommended_indices = rng.integers(0, recommended.size, size=(batch, recommended.size))
        default_variances = np.var(default[default_indices], axis=1, ddof=1)
        recommended_variances = np.var(recommended[recommended_indices], axis=1, ddof=1)
        differences.append(default_variances - recommended_variances)
        relatives.append((default_variances - recommended_variances) / default_variances)
        remaining -= batch
    difference_samples = np.concatenate(differences)
    relative_samples = np.concatenate(relatives)
    return {
        "bootstrap_seed": seed,
        "bootstrap_repeats": repeats,
        "bootstrap_variance_difference_ci95": [float(x) for x in np.quantile(difference_samples, [0.025, 0.975])],
        "bootstrap_relative_reduction_ci95": [float(x) for x in np.quantile(relative_samples, [0.025, 0.975])],
    }


def load_summary(summary_path: Path) -> tuple[np.ndarray, np.ndarray]:
    phases: list[float] = []
    variances: list[float] = []
    with summary_path.open(newline="", encoding="utf-8") as handle:
        for row in csv.DictReader(handle):
            phases.append(float(row["phase_rad"]))
            variances.append(float(row["all_variance"]))
    return np.asarray(phases), np.asarray(variances)


def write_decision_report(path: Path, result: dict[str, object], source_array: Path, channel: int) -> None:
    path.write_text(
        "# QAU measured pump-phase calibration decision\n\n"
        "This report was generated from public measured traveling-wave parametric-amplifier records. "
        "The C++ engine records both the observed phase-sweep decision and a deterministic odd/even "
        "validation diagnostic. When the selection policy is `all_observations`, the validation value "
        "is not an independent selection test; it is reported to make that limitation explicit.\n\n"
        f"- Source array: `{source_array}`\n"
        f"- Measured channel: `{channel}`\n"
        f"- Selection policy: `{result['selection_policy']}`\n"
        f"- Default phase index: `{result['default_phase_index']}`\n"
        f"- Recommended phase index: `{result['recommended_phase_index']}`\n"
        f"- Recommended run ID: `{result['recommended_run_id']}`\n"
        f"- Default phase (rad): `{result['default_phase_rad']:.17g}`\n"
        f"- Recommended phase (rad): `{result['recommended_phase_rad']:.17g}`\n"
        f"- Observed variance reduction: `{100.0 * result['observed_relative_reduction']:.6g}%`\n"
        f"- Observed reduction: `{result['observed_reduction_db']:.6g} dB`\n"
        f"- Validation variance reduction: `{100.0 * result['validation_relative_reduction']:.6g}%`\n"
        f"- Validation reduction: `{result['validation_reduction_db']:.6g} dB`\n"
        f"- Validation gate: **{'PASS' if result['validation_gate_passed'] else 'FAIL'}**\n"
        f"- Bootstrap variance-difference 95% CI: `{result['bootstrap_variance_difference_ci95']}`\n"
        f"- Bootstrap relative-reduction 95% CI: `{result['bootstrap_relative_reduction_ci95']}`\n\n"
        "> This is a measured-data operating-point recommendation for a phase-sensitive quantum readout. "
        "It is not a claim that QAU is a new physical law or a replacement for quantum optics. A live "
        "closed-loop actuator test remains a separate laboratory deployment gate.\n",
        encoding="utf-8",
    )


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--array", type=Path, required=True)
    parser.add_argument("--calibrator", type=Path, required=True)
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--channel", type=int, default=0)
    parser.add_argument("--default-phase-index", type=int, default=1)
    parser.add_argument("--selection", choices=("all", "train"), default="all")
    parser.add_argument("--bootstrap-repeats", type=int, default=1000)
    args = parser.parse_args()

    args.output_dir.mkdir(parents=True, exist_ok=True)
    input_csv = args.output_dir / "measured_phase_channel.csv"
    summary_csv = args.output_dir / "phase_summary.csv"
    result_json = args.output_dir / "calibration_result.json"
    plot_path = args.output_dir / "phase_variance.png"
    report_path = args.output_dir / "operator_decision.md"

    samples = export_channel_csv(args.array, input_csv, args.channel)
    completed = subprocess.run([
        str(args.calibrator),
        "--input", str(input_csv),
        "--summary", str(summary_csv),
        "--result", str(result_json),
        "--default-phase-index", str(args.default_phase_index),
        "--selection", args.selection,
    ], check=False)
    if completed.returncode not in (0, 2):
        raise SystemExit(completed.returncode)

    result = json.loads(result_json.read_text(encoding="utf-8"))
    measured = np.load(args.array, mmap_mode="r")
    selected_values = np.asarray(measured[:, args.channel, :], dtype=np.float64)
    bootstrap_values = selected_values if args.selection == "all" else selected_values[:, 1::2]
    result.update(bootstrap_variance_difference(
        bootstrap_values,
        int(result["default_phase_index"]) - 1,
        int(result["recommended_phase_index"]) - 1,
        args.bootstrap_repeats,
        20260812,
    ))
    result["bootstrap_source"] = "all_observations" if args.selection == "all" else "validation_odd_samples"
    result_json.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    phases, variances = load_summary(summary_csv)
    figure, axis = plt.subplots(figsize=(8.5, 4.8), constrained_layout=True)
    axis.plot(phases, variances, marker="o", linewidth=1.4, markersize=4.0, label="Measured variance")
    axis.axvline(float(result["default_phase_rad"]), color="tab:gray", linestyle="--", label="Default phase")
    axis.axvline(float(result["recommended_phase_rad"]), color="tab:red", linestyle="--", label="Recommended phase")
    axis.set_xlabel("Pump phase (rad)")
    axis.set_ylabel("Measured sample variance")
    axis.set_title(f"Real measured phase calibration, channel {args.channel}, n={samples:,} per setting")
    axis.grid(True, alpha=0.25)
    axis.legend()
    figure.savefig(plot_path, dpi=160)
    plt.close(figure)
    write_decision_report(report_path, result, args.array, args.channel)
    print(f"wrote {input_csv}")
    print(f"wrote {summary_csv}")
    print(f"wrote {result_json}")
    print(f"wrote {plot_path}")
    print(f"wrote {report_path}")
    return completed.returncode


if __name__ == "__main__":
    raise SystemExit(main())
