#!/usr/bin/env python3
from __future__ import annotations
import argparse
from pathlib import Path
import sys
sys.path.insert(0, str(Path(__file__).resolve().parent))
from parse_sqlite_master_pages import page_info, varint


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("page", type=Path)
    args = parser.parse_args()
    page = args.page.read_bytes()
    page_type, pointers, _ = page_info(page, 2)
    print(f"type={page_type} pointers={pointers}")
    for pointer in pointers:
        payload_len, cursor = varint(page, pointer)
        rowid, cursor = varint(page, cursor)
        payload = page[cursor:min(len(page), cursor + payload_len)]
        print(f"offset={pointer} payload_len={payload_len} rowid={rowid} local={len(payload)} payload_hex={payload[:48].hex()}")
        if payload:
            header_len, hcursor = varint(payload, 0)
            serials = []
            while hcursor < header_len:
                serial, hcursor = varint(payload, hcursor)
                serials.append(serial)
            lengths = []
            for serial in serials:
                if serial >= 12:
                    lengths.append((serial - 12) // 2 if serial % 2 == 0 else (serial - 13) // 2)
                else:
                    lengths.append(0)
            print(f"  header_len={header_len} serials={serials} blob_or_text_lengths={lengths} header_hex={payload[:header_len].hex()} data_prefix={payload[header_len:header_len+32].hex()}")
    return 0

if __name__ == "__main__":
    raise SystemExit(main())
