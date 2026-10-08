#!/usr/bin/env python3
"""
Genereaza fixture-urile ITCH folosite de testul de comparatie cu oracolul.

    python3 tests/make_fixture.py data/messagesSpecs.json tests/data

Produce, determinist (seed fix):
  fixture.itch            -- 3 mesaje din fiecare tip din specificatii, cu valori
                             aleatoare, plus un mesaj de tip necunoscut ('Z')
  fixture_truncated.itch  -- acelasi fisier, cu ultimii 5 bytes taiati

Se ruleaza doar cand se schimba specificatiile; fisierele rezultate se pun in git.
"""
import json
import random
import struct
import sys

PRINTABLE = b"ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 "


def make_message(rng, letter, spec):
    fmt = spec["FORMAT"]
    length = struct.calcsize(fmt)
    assert length == spec["LENGTH"], letter

    raw = bytes(rng.getrandbits(8) for _ in range(length))
    values = list(struct.unpack(fmt, raw))
    values[0] = letter.encode("ascii")
    for i in spec["ASCII_FIELDS"][1:]:
        values[i] = bytes(rng.choice(PRINTABLE) for _ in range(len(values[i])))
    for i in spec["UINT48_FIELDS"]:
        values[i] = rng.getrandbits(48).to_bytes(6, "big")
    return struct.pack(fmt, *values)


def frame(body):
    return struct.pack(">H", len(body)) + body


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: make_fixture.py <messagesSpecs.json> <output_dir>")
    specs_path, out_dir = sys.argv[1:]
    with open(specs_path) as f:
        specs = json.load(f)

    rng = random.Random(20260101)
    data = b""
    for letter in sorted(specs):
        for _ in range(3):
            data += frame(make_message(rng, letter, specs[letter]))
    data += frame(b"Z" + bytes(9))   # tip necunoscut: trebuie sarit

    with open(f"{out_dir}/fixture.itch", "wb") as f:
        f.write(data)
    with open(f"{out_dir}/fixture_truncated.itch", "wb") as f:
        f.write(data[:-5])
    print(f"fixture.itch: {len(data)} bytes, {3 * len(specs)} messages + 1 unknown")


if __name__ == "__main__":
    main()