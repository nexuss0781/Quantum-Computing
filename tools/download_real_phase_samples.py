#!/usr/bin/env python3
"""Extract a bounded sample from the public measured QCoDeS SQLite database.

The Zenodo object is a 25.6 GB SQLite database. This script deliberately uses HTTP
range requests and SQLite overflow-page decoding so a normal calibration run does
not download or modify the complete archive.
"""
from __future__ import annotations

import argparse
import csv
import hashlib
import json
import math
import re
import struct
import time
import urllib.request
from pathlib import Path

import numpy as np

PAGE_SIZE = 4096
OVERFLOW_PAYLOAD = PAGE_SIZE - 4
DEFAULT_URL = "https://zenodo.org/api/records/5217997/files/Raw_data_2_mode_squeezing/content"


def decode_varint(data: bytes, offset: int) -> tuple[int, int]:
    value = 0
    for index in range(9):
        byte = data[offset + index]
        if index == 8:
            return (value << 8) | byte, offset + 9
        value = (value << 7) | (byte & 0x7F)
        if byte < 0x80:
            return value, offset + index + 1
    raise ValueError("invalid SQLite varint")


def fetch_range(url: str, start: int, end: int) -> bytes:
    # Keep requests below a few MiB so a slow public object store does not
    # invalidate an otherwise successful extraction. Retries are bounded and
    # never substitute generated data.
    chunk_size = 4 * 1024 * 1024
    pieces: list[bytes] = []
    cursor = start
    while cursor <= end:
        chunk_end = min(end, cursor + chunk_size - 1)
        expected = chunk_end - cursor + 1
        last_error: Exception | None = None
        for attempt in range(4):
            try:
                request = urllib.request.Request(url, headers={"Range": f"bytes={cursor}-{chunk_end}"})
                with urllib.request.urlopen(request, timeout=240) as response:
                    payload = response.read()
                    content_range = response.headers.get("Content-Range", "")
                if len(payload) != expected:
                    raise RuntimeError(f"short range {cursor}-{chunk_end}: got {len(payload)} expected {expected}; {content_range}")
                pieces.append(payload)
                last_error = None
                break
            except Exception as exc:  # public object stores can transiently stall
                last_error = exc
                time.sleep(2.0 * (attempt + 1))
        if last_error is not None:
            raise RuntimeError(f"failed range {cursor}-{chunk_end}: {last_error}") from last_error
        cursor = chunk_end + 1
    return b"".join(pieces)


def fetch_page(url: str, page_number: int) -> bytes:
    start = (page_number - 1) * PAGE_SIZE
    return fetch_range(url, start, start + PAGE_SIZE - 1)


def parse_schema_roots(schema_file: Path) -> dict[int, int]:
    roots: dict[int, int] = {}
    pattern = re.compile(r"\['table', 'results-3-(\d+)', 'results-3-\d+', (\d+),")
    for line in schema_file.read_text(encoding="utf-8").splitlines():
        match = pattern.search(line)
        if match:
            roots[int(match.group(1))] = int(match.group(2))
    expected = set(range(1, 26))
    missing = expected.difference(roots)
    if missing:
        raise RuntimeError(f"schema is missing phase tables: {sorted(missing)}")
    return roots


def locate_blob(url: str, root_page: int) -> tuple[bytes, int, int, int]:
    page = fetch_page(url, root_page)
    page_type = page[0]
    if page_type != 13:
        raise RuntimeError(f"phase root page {root_page} is not a table leaf: {page_type}")
    cell_count = int.from_bytes(page[3:5], "big")
    pointers = [int.from_bytes(page[8 + 2 * i:10 + 2 * i], "big") for i in range(cell_count)]
    candidates: list[tuple[int, int, int, int]] = []
    for pointer in pointers:
        payload_length, cursor = decode_varint(page, pointer)
        row_id, cursor = decode_varint(page, cursor)
        if payload_length > 1_000_000:
            local_length = PAGE_SIZE - 4 - cursor
            overflow_page = int.from_bytes(page[-4:], "big")
            local_payload = page[cursor:cursor + local_length]
            candidates.append((row_id, payload_length, overflow_page, len(local_payload)))
    if len(candidates) != 1:
        raise RuntimeError(f"expected one large blob in root page {root_page}, got {candidates}")
    row_id, payload_length, overflow_page, local_length = candidates[0]
    return page, row_id, payload_length, overflow_page


def read_blob_prefix(url: str, root_page: int, needed_bytes: int) -> tuple[bytes, dict[str, int]]:
    # Fetch a small record prefix beyond the requested NumPy bytes because the
    # SQLite record header precedes the embedded NPY payload.
    record_prefix_needed = needed_bytes + 64
    page, row_id, payload_length, first_overflow = locate_blob(url, root_page)
    cell_count = int.from_bytes(page[3:5], "big")
    pointers = [int.from_bytes(page[8 + 2 * i:10 + 2 * i], "big") for i in range(cell_count)]
    pointer = None
    cursor = None
    for candidate in pointers:
        total, candidate_cursor = decode_varint(page, candidate)
        candidate_row, candidate_cursor = decode_varint(page, candidate_cursor)
        if candidate_row == row_id and total == payload_length:
            pointer = candidate
            cursor = candidate_cursor
            break
    if pointer is None or cursor is None:
        raise RuntimeError("could not recover large blob cell")
    local_length = PAGE_SIZE - 4 - cursor
    local_payload = page[cursor:cursor + local_length]
    if first_overflow <= 0:
        raise RuntimeError("large blob has no overflow page")

    # The public database places this row's overflow pages contiguously. Verify
    # that property on the first page before issuing one bounded range request.
    first_page = fetch_page(url, first_overflow)
    next_page = int.from_bytes(first_page[:4], "big")
    if next_page not in (0, first_overflow + 1):
        raise RuntimeError(f"non-contiguous overflow chain: {first_overflow} -> {next_page}")
    remaining = max(0, record_prefix_needed - len(local_payload))
    page_count = math.ceil(remaining / OVERFLOW_PAYLOAD)
    start = (first_overflow - 1) * PAGE_SIZE
    end = start + page_count * PAGE_SIZE - 1
    raw_pages = fetch_range(url, start, end) if page_count else b""
    payload_parts = [local_payload]
    for index in range(page_count):
        page_start = index * PAGE_SIZE
        overflow_page_bytes = raw_pages[page_start:page_start + PAGE_SIZE]
        payload_parts.append(overflow_page_bytes[4:])
    record_prefix = b"".join(payload_parts)[:record_prefix_needed]
    if len(record_prefix) < 16:
        raise RuntimeError(f"record prefix too short: {len(record_prefix)}")
    header_length, header_cursor = decode_varint(record_prefix, 0)
    serials: list[int] = []
    while header_cursor < header_length:
        serial, header_cursor = decode_varint(record_prefix, header_cursor)
        serials.append(serial)
    data_cursor = header_length
    blob_start = None
    for serial in serials:
        if serial >= 12:
            length = (serial - 12) // 2 if serial % 2 == 0 else (serial - 13) // 2
            if serial % 2 == 0 and blob_start is None:
                blob_start = data_cursor
            data_cursor += length
        elif serial in (1, 2, 3, 4, 5, 6, 7):
            data_cursor += {1: 1, 2: 2, 3: 3, 4: 4, 5: 6, 6: 8, 7: 8}[serial]
    if blob_start is None:
        raise RuntimeError(f"no blob column in SQLite record serials={serials}")
    prefix = record_prefix[blob_start:blob_start + needed_bytes]
    if len(prefix) != needed_bytes:
        raise RuntimeError(f"blob prefix too short: {len(prefix)} != {needed_bytes}")
    return prefix, {
        "row_id": row_id,
        "payload_length": payload_length,
        "first_overflow_page": first_overflow,
        "overflow_pages_fetched": page_count,
        "local_payload_bytes": local_length,
    }


def parse_npy_prefix(blob_prefix: bytes, samples: int, channels: int) -> tuple[np.ndarray, dict[str, object]]:
    from io import BytesIO
    from numpy.lib import format as npy_format

    stream = BytesIO(blob_prefix)
    major, minor = npy_format.read_magic(stream)
    if (major, minor) == (1, 0):
        shape, fortran_order, dtype = npy_format.read_array_header_1_0(stream)
    elif (major, minor) == (2, 0):
        shape, fortran_order, dtype = npy_format.read_array_header_2_0(stream)
    else:
        raise RuntimeError(f"unsupported NPY version {(major, minor)}")
    if fortran_order or tuple(shape[:-1]) != (2, 2, 2) or dtype != np.dtype("<f8"):
        raise RuntimeError(f"unexpected measured array header shape={shape} fortran={fortran_order} dtype={dtype}")
    if shape[-1] < samples:
        raise RuntimeError(f"dataset contains only {shape[-1]} samples")
    data_offset = stream.tell()
    required = data_offset + channels * samples * dtype.itemsize
    if len(blob_prefix) < required:
        raise RuntimeError(f"NPY prefix too short for requested data: {len(blob_prefix)} < {required}")
    raw = blob_prefix[data_offset:required]
    array = np.frombuffer(raw, dtype=dtype).reshape(channels, samples)
    metadata = {
        "npy_version": f"{major}.{minor}",
        "shape": list(shape),
        "fortran_order": bool(fortran_order),
        "dtype": dtype.str,
        "data_offset": data_offset,
    }
    return array, metadata


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--schema", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    parser.add_argument("--url", default=DEFAULT_URL)
    parser.add_argument("--samples", type=int, default=100_000)
    parser.add_argument("--channels", type=int, default=8)
    args = parser.parse_args()
    if args.samples <= 0 or args.channels != 8:
        raise SystemExit("this extractor currently requires positive samples and channels=8")

    roots = parse_schema_roots(args.schema)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    metadata_rows: list[dict[str, object]] = []
    required_prefix = 1_000_000 + args.channels * args.samples * 8
    # 1 MB covers the NPY header and local payload; the bounded range is still
    # tiny compared with the 25.6 GB source database.
    with args.output.open("w", newline="", encoding="utf-8") as handle:
        writer = csv.writer(handle)
        writer.writerow(["run_id", "phase_index", "phase_rad", "channel", "sample_index", "value"])
        for phase_index in range(1, 26):
            root_page = roots[phase_index]
            blob_prefix, blob_meta = read_blob_prefix(args.url, root_page, required_prefix)
            channels, npy_meta = parse_npy_prefix(blob_prefix, args.samples, args.channels)
            phase_rad = math.pi * (phase_index - 1) / 24.0
            run_id = phase_index + 10
            for channel_index in range(args.channels):
                values = np.asarray(channels[channel_index], dtype=np.float64)
                for sample_index, value in enumerate(values):
                    writer.writerow([
                        run_id,
                        phase_index,
                        f"{phase_rad:.15g}",
                        channel_index,
                        sample_index,
                        f"{float(value):.17g}",
                    ])
            metadata_rows.append({
                "run_id": run_id,
                "phase_index": phase_index,
                "phase_rad": phase_rad,
                "root_page": root_page,
                **blob_meta,
                **npy_meta,
                "samples_per_channel": args.samples,
                "channels": args.channels,
            })
            print(f"extracted phase_index={phase_index} run_id={run_id} phase_rad={phase_rad:.12f} root_page={root_page}")

    metadata_path = args.output.with_suffix(".metadata.json")
    metadata_path.write_text(json.dumps({
        "source_url": args.url,
        "source_record": "https://doi.org/10.5281/zenodo.5217997",
        "source_archive_md5": "59590d1cdf5fead5283493764164c9f1",
        "source_archive_size_bytes": 25626624000,
        "samples_per_channel": args.samples,
        "channels": args.channels,
        "phase_runs": metadata_rows,
        "csv_sha256": hashlib.sha256(args.output.read_bytes()).hexdigest(),
    }, indent=2) + "\n", encoding="utf-8")
    print(f"wrote {args.output} ({args.output.stat().st_size} bytes)")
    print(f"wrote {metadata_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
