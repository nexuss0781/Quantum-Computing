#!/usr/bin/env python3
"""Inspect a partially downloaded SQLite database without modifying it."""
from __future__ import annotations

import argparse
import sqlite3
from pathlib import Path


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("database", type=Path)
    args = parser.parse_args()
    uri = f"file:{args.database.resolve()}?mode=ro"
    try:
        connection = sqlite3.connect(uri, uri=True)
        rows = connection.execute(
            "SELECT type, name, tbl_name, rootpage, sql "
            "FROM sqlite_master ORDER BY type, name"
        ).fetchall()
        for row in rows:
            print(row)
        connection.close()
        return 0
    except sqlite3.Error as exc:
        print(f"sqlite-error: {exc}")
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
