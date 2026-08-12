#!/usr/bin/env python3
"""Analyze real measured phase-array channels; never generates measurements."""
from __future__ import annotations

import argparse
import json
from pathlib import Path

import numpy as np


def bootstrap_reduction(default: np.ndarray, recommended: np.ndarray, repeats: int, seed: int) -> tuple[float, float, float]:
    rng = np.random.default_rng(seed)
    reductions = np.empty(repeats, dtype=np.float64)
    n_default = len(default)
    n_recommended = len(recommended)
    for index in range(repeats):
        default_sample = default[rng.integers(0, n_default, n_default)]
        recommended_sample = recommended[rng.integers(0, n_recommended, n_recommended)]
        default_variance = float(np.var(default_sample, ddof=1))
        recommended_variance = float(np.var(recommended_sample, ddof=1))
        reductions[index] = (default_variance - recommended_variance) / default_variance
    return tuple(float(value) for value in np.quantile(reductions, [0.025, 0.5, 0.975]))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("array", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--bootstrap", type=int, default=1000)
    args = parser.parse_args()

    measured = np.load(args.array, mmap_mode="r")
    if measured.ndim != 3 or measured.shape[0] != 25 or measured.shape[1] != 8:
        raise SystemExit(f"expected (25, 8, samples), got {measured.shape}")
    records = []
    for channel in range(measured.shape[1]):
        values = np.asarray(measured[:, channel, :], dtype=np.float64)
        train = values[:, ::2]
        validation = values[:, 1::2]
        train_variance = np.var(train, axis=1, ddof=1)
        validation_variance = np.var(validation, axis=1, ddof=1)
        recommended_index = int(np.argmin(train_variance))
        default_index = 0
        default_train = float(train_variance[default_index])
        recommended_train = float(train_variance[recommended_index])
        default_validation = float(validation_variance[default_index])
        recommended_validation = float(validation_variance[recommended_index])
        ci_low, ci_median, ci_high = bootstrap_reduction(
            values[default_index, 1::2],
            values[recommended_index, 1::2],
            args.bootstrap,
            20260812 + channel,
        )
        records.append({
            "channel": channel,
            "sample_count_per_phase": int(values.shape[1]),
            "default_phase_index": 1,
            "recommended_phase_index": recommended_index + 1,
            "recommended_run_id": recommended_index + 11,
            "default_phase_rad": 0.0,
            "recommended_phase_rad": float(np.pi * recommended_index / 24.0),
            "default_train_variance": default_train,
            "recommended_train_variance": recommended_train,
            "default_validation_variance": default_validation,
            "recommended_validation_variance": recommended_validation,
            "training_relative_reduction": (default_train - recommended_train) / default_train,
            "validation_relative_reduction": (default_validation - recommended_validation) / default_validation,
            "validation_reduction_db": float(10.0 * np.log10(default_validation / recommended_validation)),
            "bootstrap_relative_reduction_ci95": [ci_low, ci_high],
            "bootstrap_relative_reduction_median": ci_median,
            "validation_gate_passed": bool(recommended_validation < default_validation),
        })
    result = {
        "input": str(args.array),
        "shape": list(measured.shape),
        "bootstrap_repeats": args.bootstrap,
        "records": records,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    for record in records:
        print(json.dumps({
            key: record[key]
            for key in (
                "channel", "recommended_phase_rad", "recommended_run_id",
                "default_validation_variance", "recommended_validation_variance",
                "validation_relative_reduction", "validation_reduction_db",
                "bootstrap_relative_reduction_ci95", "validation_gate_passed",
            )
        }))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
