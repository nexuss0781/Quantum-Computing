#!/usr/bin/env python3
"""Summarize measured pump-phase samples without generating any data."""
from __future__ import annotations

import argparse
import csv
import json
from collections import defaultdict
from pathlib import Path

import numpy as np


def load(path: Path) -> dict[tuple[int, int], np.ndarray]:
    groups: dict[tuple[int, int], list[float]] = defaultdict(list)
    with path.open(newline="", encoding="utf-8") as handle:
        for row in csv.DictReader(handle):
            groups[(int(row["phase_index"]), int(row["channel"]))].append(float(row["value"]))
    return {key: np.asarray(values, dtype=np.float64) for key, values in groups.items()}


def bootstrap_reduction(default: np.ndarray, recommended: np.ndarray, repeats: int, seed: int) -> tuple[float, float, float]:
    rng = np.random.default_rng(seed)
    n_default = len(default)
    n_recommended = len(recommended)
    reductions = np.empty(repeats, dtype=np.float64)
    for index in range(repeats):
        d = default[rng.integers(0, n_default, n_default)]
        r = recommended[rng.integers(0, n_recommended, n_recommended)]
        dvar = float(np.var(d, ddof=1))
        rvar = float(np.var(r, ddof=1))
        reductions[index] = (dvar - rvar) / dvar
    return tuple(float(x) for x in np.quantile(reductions, [0.025, 0.5, 0.975]))


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("input", type=Path)
    parser.add_argument("--channel", type=int, default=0)
    parser.add_argument("--default-phase-index", type=int, default=1)
    parser.add_argument("--bootstrap", type=int, default=2000)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()

    groups = load(args.input)
    phases = sorted({phase for phase, channel in groups if channel == args.channel})
    if not phases:
        raise SystemExit(f"no data for channel {args.channel}")
    rows = []
    for phase in phases:
        values = groups[(phase, args.channel)]
        variance = float(np.var(values, ddof=1))
        mean = float(np.mean(values))
        rows.append({
            "phase_index": phase,
            "run_id": phase + 10,
            "phase_rad": float(np.pi * (phase - 1) / 24.0),
            "channel": args.channel,
            "sample_count": int(len(values)),
            "mean": mean,
            "variance": variance,
            "stddev": float(np.sqrt(variance)),
        })
    recommended = min(rows, key=lambda row: row["variance"])
    default = next(row for row in rows if row["phase_index"] == args.default_phase_index)
    default_values = groups[(args.default_phase_index, args.channel)]
    recommended_values = groups[(recommended["phase_index"], args.channel)]
    reduction = (default["variance"] - recommended["variance"]) / default["variance"]
    reduction_db = 10.0 * np.log10(default["variance"] / recommended["variance"])
    ci_low, ci_median, ci_high = bootstrap_reduction(default_values, recommended_values, args.bootstrap, 20260812)
    result = {
        "input": str(args.input),
        "channel": args.channel,
        "default": default,
        "recommended": recommended,
        "relative_variance_reduction": float(reduction),
        "relative_variance_reduction_percent": float(100.0 * reduction),
        "reduction_db": float(reduction_db),
        "bootstrap_repeats": args.bootstrap,
        "bootstrap_seed": 20260812,
        "bootstrap_relative_reduction_ci95": [ci_low, ci_high],
        "bootstrap_relative_reduction_median": ci_median,
        "phase_table": rows,
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(result, indent=2) + "\n", encoding="utf-8")
    print(json.dumps({
        "channel": args.channel,
        "default_phase_rad": default["phase_rad"],
        "recommended_phase_rad": recommended["phase_rad"],
        "recommended_run_id": recommended["run_id"],
        "default_variance": default["variance"],
        "recommended_variance": recommended["variance"],
        "relative_reduction_percent": 100.0 * reduction,
        "reduction_db": float(reduction_db),
        "bootstrap_ci95": [ci_low, ci_high],
    }, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
