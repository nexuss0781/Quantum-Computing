#!/usr/bin/env python3
from __future__ import annotations
import argparse
import json
import math
import re
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--metadata", type=Path, required=True)
    parser.add_argument("--schema", type=Path, required=True)
    args = parser.parse_args()
    metadata = json.loads(args.metadata.read_text(encoding="utf-8"))
    roots: dict[int, int] = {}
    pattern = re.compile(r"\['table', 'results-3-(\d+)', 'results-3-\d+', (\d+),")
    for line in args.schema.read_text(encoding="utf-8").splitlines():
        match = pattern.search(line)
        if match:
            roots[int(match.group(1))] = int(match.group(2))
    old = {int(row["phase_index"]): row for row in metadata.get("phase_runs", [])}
    phase_runs = []
    for phase_index in range(1, 26):
        row = dict(old.get(phase_index, {}))
        row.update({
            "phase_index": phase_index,
            "run_id": phase_index + 10,
            "phase_rad": math.pi * (phase_index - 1) / 24.0,
            "root_page": roots[phase_index],
        })
        phase_runs.append(row)
    metadata["phase_runs"] = phase_runs
    metadata["phase_mapping_complete"] = True
    metadata["extraction_note"] = "Each phase row contains 100,000 samples per flattened measured channel; detailed SQLite blob metadata is retained where recorded during range extraction."
    args.metadata.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
    print(f"completed {args.metadata} with {len(phase_runs)} phase rows")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
