#!/usr/bin/env python3
"""Prepare the minimal sparse SQLite schema view needed for the real phase sweep."""
from __future__ import annotations

import argparse
import subprocess
import sys
from pathlib import Path

from download_real_phase_samples import DEFAULT_URL, PAGE_SIZE, fetch_range
from parse_sqlite_master_pages import page_info

SOURCE_SIZE = 25_626_624_000


def write_at(path: Path, offset: int, data: bytes) -> None:
    with path.open("r+b") as handle:
        handle.seek(offset)
        handle.write(data)


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--output-dir", type=Path, required=True)
    parser.add_argument("--schema-output", type=Path, required=True)
    parser.add_argument("--url", default=DEFAULT_URL)
    args = parser.parse_args()
    args.output_dir.mkdir(parents=True, exist_ok=True)
    header_path = args.output_dir / "archive_schema.bin"
    sparse_path = args.output_dir / "measurement_sparse.sqlite"

    header = fetch_range(args.url, 0, 1_048_575)
    header_path.write_bytes(header)
    root = header[:PAGE_SIZE]
    page_type, pointers, rightmost = page_info(root, 1)
    if page_type != 5:
        raise RuntimeError(f"expected sqlite_master interior root page, got {page_type}")
    child_pages = [int.from_bytes(root[pointer:pointer + 4], "big") for pointer in pointers]
    child_pages.append(rightmost)

    with sparse_path.open("wb") as handle:
        handle.truncate(SOURCE_SIZE)
    write_at(sparse_path, 0, header)
    for page_number in child_pages:
        page = fetch_range(args.url, (page_number - 1) * PAGE_SIZE, page_number * PAGE_SIZE - 1)
        write_at(sparse_path, (page_number - 1) * PAGE_SIZE, page)

    parser_path = Path(__file__).with_name("parse_sqlite_master_pages.py")
    with args.schema_output.open("w", encoding="utf-8") as output:
        completed = subprocess.run(
            [sys.executable, str(parser_path), str(sparse_path)],
            check=False,
            stdout=output,
            stderr=subprocess.PIPE,
            text=True,
        )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr.strip() or "schema parser failed")
    print(f"wrote {header_path}")
    print(f"wrote sparse schema view {sparse_path}")
    print(f"wrote {args.schema_output}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
