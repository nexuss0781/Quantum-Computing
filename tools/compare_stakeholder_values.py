#!/usr/bin/env python3
from __future__ import annotations
import argparse
import json
from pathlib import Path
import numpy as np


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("array", type=Path)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    measured = np.load(args.array, mmap_mode="r")
    rows = []
    for channel in range(measured.shape[1]):
        default = np.asarray(measured[0, channel], dtype=np.float64)
        run27 = np.asarray(measured[16, channel], dtype=np.float64)
        all_variance_default = float(np.var(default, ddof=1))
        all_variance_run27 = float(np.var(run27, ddof=1))
        rows.append({
            "channel": channel,
            "default_variance": all_variance_default,
            "run27_variance": all_variance_run27,
            "reduction_percent": 100.0 * (all_variance_default - all_variance_run27) / all_variance_default,
            "default_difference_from_stakeholder": all_variance_default - 5.523737e-7,
            "run27_difference_from_stakeholder": all_variance_run27 - 5.423479e-7,
        })
    args.output.write_text(json.dumps(rows, indent=2) + "\n", encoding="utf-8")
    for row in rows:
        print(row)
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
