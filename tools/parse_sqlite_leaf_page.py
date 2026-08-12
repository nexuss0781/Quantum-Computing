#!/usr/bin/env python3
"""Decode SQLite table-leaf cells from one fetched 4096-byte page."""
from __future__ import annotations

import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
from parse_sqlite_master_pages import extract_leaf_records, page_info  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("page", type=Path)
    args = parser.parse_args()
    data = args.page.read_bytes()
    page_type, pointers, rightmost = page_info(data, 2)
    print(f"type={page_type} cells={len(pointers)} pointers={pointers} rightmost={rightmost}")
    for index, record in enumerate(extract_leaf_records(data, 2), start=1):
        print(f"row {index}: {record}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
