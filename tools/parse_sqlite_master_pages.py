#!/usr/bin/env python3
"""Extract SQLite sqlite_master records from selected pages of a sparse DB."""
from __future__ import annotations

import argparse
import struct
from pathlib import Path

PAGE_SIZE = 4096


def varint(data: bytes, offset: int) -> tuple[int, int]:
    value = 0
    for i in range(9):
        byte = data[offset + i]
        if i == 8:
            value = (value << 8) | byte
            return value, offset + 9
        value = (value << 7) | (byte & 0x7F)
        if byte < 0x80:
            return value, offset + i + 1
    raise ValueError("unreachable")


def read_page(handle, page_number: int) -> bytes:
    handle.seek((page_number - 1) * PAGE_SIZE)
    page = handle.read(PAGE_SIZE)
    if len(page) != PAGE_SIZE:
        raise ValueError(f"short page {page_number}: {len(page)} bytes")
    return page


def page_info(page: bytes, page_number: int) -> tuple[int, list[int], int]:
    base = 100 if page_number == 1 else 0
    page_type = page[base]
    cell_count = int.from_bytes(page[base + 3:base + 5], "big")
    header_size = 12 if page_type in (2, 5) else 8
    pointers = [
        int.from_bytes(page[base + header_size + 2 * i:base + header_size + 2 + 2 * i], "big")
        for i in range(cell_count)
    ]
    rightmost = int.from_bytes(page[base + 8:base + 12], "big") if page_type in (2, 5) else 0
    return page_type, pointers, rightmost


def parse_record(payload: bytes) -> list[object]:
    header_len, cursor = varint(payload, 0)
    types: list[int] = []
    while cursor < header_len:
        serial, cursor = varint(payload, cursor)
        types.append(serial)
    data_cursor = header_len
    values: list[object] = []
    for serial in types:
        if serial == 0:
            values.append(None)
        elif serial == 1:
            values.append(int.from_bytes(payload[data_cursor:data_cursor + 1], "big", signed=True))
            data_cursor += 1
        elif serial == 2:
            values.append(int.from_bytes(payload[data_cursor:data_cursor + 2], "big", signed=True))
            data_cursor += 2
        elif serial == 3:
            raw = payload[data_cursor:data_cursor + 3]
            sign = b"\xff" if raw[0] & 0x80 else b"\x00"
            values.append(int.from_bytes(sign + raw, "big", signed=True))
            data_cursor += 3
        elif serial == 4:
            values.append(int.from_bytes(payload[data_cursor:data_cursor + 4], "big", signed=True))
            data_cursor += 4
        elif serial == 5:
            raw = payload[data_cursor:data_cursor + 6]
            sign = b"\xff\xff" if raw[0] & 0x80 else b"\x00\x00"
            values.append(int.from_bytes(sign + raw, "big", signed=True))
            data_cursor += 6
        elif serial == 6:
            values.append(int.from_bytes(payload[data_cursor:data_cursor + 8], "big", signed=True))
            data_cursor += 8
        elif serial == 7:
            values.append(struct.unpack(">d", payload[data_cursor:data_cursor + 8])[0])
            data_cursor += 8
        elif serial == 8:
            values.append(0)
        elif serial == 9:
            values.append(1)
        elif serial >= 12:
            length = (serial - 12) // 2 if serial % 2 == 0 else (serial - 13) // 2
            raw = payload[data_cursor:data_cursor + length]
            values.append(raw if serial % 2 == 0 else raw.decode("utf-8", errors="replace"))
            data_cursor += length
        else:
            raise ValueError(f"unsupported serial type {serial}")
    return values


def extract_leaf_records(page: bytes, page_number: int) -> list[list[object]]:
    page_type, pointers, _ = page_info(page, page_number)
    if page_type != 13:
        raise ValueError(f"page {page_number} is not a table leaf: type={page_type}")
    records: list[list[object]] = []
    for cell_offset in pointers:
        payload_len, cursor = varint(page, cell_offset)
        _rowid, cursor = varint(page, cursor)
        payload = page[cursor:cursor + payload_len]
        if len(payload) != payload_len:
            raise ValueError(f"payload outside page {page_number}")
        records.append(parse_record(payload))
    return records


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("database", type=Path)
    args = parser.parse_args()
    with args.database.open("rb") as handle:
        root = read_page(handle, 1)
        root_type, pointers, rightmost = page_info(root, 1)
        print(f"root type={root_type} cell_count={len(pointers)} rightmost={rightmost}")
        child_pages: list[int] = []
        for pointer in pointers:
            child_pages.append(int.from_bytes(root[pointer:pointer + 4], "big"))
        child_pages.append(rightmost)
        print(f"child_pages={child_pages}")
        for page_number in child_pages:
            page = read_page(handle, page_number)
            page_type, cell_ptrs, _ = page_info(page, page_number)
            print(f"page={page_number} type={page_type} cell_count={len(cell_ptrs)}")
            if page_type == 13:
                for record in extract_leaf_records(page, page_number):
                    print(record)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
