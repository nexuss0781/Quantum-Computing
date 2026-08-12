#!/usr/bin/env python3
"""Extract the stakeholder-required measured phase sweep into a compact NPY array."""
from __future__ import annotations

import argparse
import hashlib
import json
import math
from pathlib import Path

import numpy as np

from download_real_phase_samples import DEFAULT_URL, parse_npy_prefix, parse_schema_roots, read_blob_prefix


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--schema", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--url", default=DEFAULT_URL)
    parser.add_argument("--samples", type=int, default=100_000)
    parser.add_argument("--channels", type=int, default=8)
    parser.add_argument("--start-phase", type=int, default=1)
    parser.add_argument("--stop-phase", type=int, default=25)
    args = parser.parse_args()
    if args.samples <= 0 or args.channels != 8 or not (1 <= args.start_phase <= args.stop_phase <= 25):
        raise SystemExit("require channels=8, positive samples, and 1 <= start-phase <= stop-phase <= 25")

    roots = parse_schema_roots(args.schema)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    mode = "r+" if args.output.exists() else "w+"
    measured = np.lib.format.open_memmap(
        args.output,
        mode=mode,
        dtype="<f8",
        shape=(25, args.channels, args.samples),
        fortran_order=False,
    )
    phase_rows: list[dict[str, object]] = []
    required_prefix = 1_000_000 + args.channels * args.samples * 8
    for phase_index in range(args.start_phase, args.stop_phase + 1):
        root_page = roots[phase_index]
        blob_prefix, blob_meta = read_blob_prefix(args.url, root_page, required_prefix)
        channels, npy_meta = parse_npy_prefix(blob_prefix, args.samples, args.channels)
        measured[phase_index - 1, :, :] = channels
        phase_rows.append({
            "phase_index": phase_index,
            "run_id": phase_index + 10,
            "phase_rad": math.pi * (phase_index - 1) / 24.0,
            "root_page": root_page,
            **blob_meta,
            **npy_meta,
        })
        measured.flush()
        print(f"extracted phase_index={phase_index} run_id={phase_index + 10} phase_rad={math.pi * (phase_index - 1) / 24.0:.12f}")
    del measured

    metadata_path = args.output.with_suffix(".metadata.json")
    metadata_path.write_text(json.dumps({
        "source_url": args.url,
        "source_record": "https://doi.org/10.5281/zenodo.5217997",
        "source_archive_md5": "59590d1cdf5fead5283493764164c9f1",
        "source_archive_size_bytes": 25626624000,
        "array_file": str(args.output),
        "array_sha256": hashlib.sha256(args.output.read_bytes()).hexdigest(),
        "shape": [25, args.channels, args.samples],
        "dtype": "<f8",
        "phase_runs": phase_rows,
    }, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {args.output} ({args.output.stat().st_size} bytes)")
    print(f"wrote {metadata_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
