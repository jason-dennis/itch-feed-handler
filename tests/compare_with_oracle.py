#!/usr/bin/env python3
"""
Test de capat la capat: C++-ul si oracolul Python trebuie sa dea acelasi rezultat.

    compare_with_oracle.py <ITCHFeedhandler> [<messagesSpecs.json> <fixture_dir> <oracle_dir> <work_dir>]

Daca se da doar executabilul, restul cailor se deduc din locul scriptului
(radacina repo-ului = folderul parinte al lui tests/).

1. fixture.itch: output-ul JSONL al C++-ului e identic cu al oracolului,
   iar contoarele parsed/skipped sunt aceleasi.
2. fixture_truncated.itch: ambele se opresc cu acelasi mesaj de eroare.
"""
import importlib
import json
import os
import subprocess
import sys
from typing import NoReturn


def fail(msg) -> NoReturn:
    print("FAIL:", msg)
    sys.exit(1)


def read_lines(path):
    with open(path, "r", newline="") as f:
        return f.read().replace("\r\n", "\n").splitlines()


def main():
    if len(sys.argv) == 2:
        root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
        exe = sys.argv[1]
        specs_path = os.path.join(root, "data", "messagesSpecs.json")
        fixture_dir = os.path.join(root, "tests", "data")
        oracle_dir = os.path.join(root, "python")
        work_dir = os.path.join(root, "tests", "out")
    elif len(sys.argv) == 6:
        exe, specs_path, fixture_dir, oracle_dir, work_dir = sys.argv[1:]
    else:
        sys.exit(__doc__)

    oracle_dir = os.path.abspath(oracle_dir)
    if not os.path.isfile(os.path.join(oracle_dir, "oracle.py")):
        fail(f"oracle.py not found in {oracle_dir}")
    if not os.path.isfile(exe):
        fail(f"executable not found: {exe}")
    os.makedirs(work_dir, exist_ok=True)

    sys.path.insert(0, oracle_dir)
    # importat dinamic din python/; oracle.decode(buffer, SPECS, f) -> (parsed, skipped)
    oracle = importlib.import_module("oracle")

    with open(specs_path) as f:
        specs = json.load(f)

    # --- 1. fixture valid ---------------------------------------------------
    fixture = os.path.join(fixture_dir, "fixture.itch")
    cpp_out = os.path.join(work_dir, "fixture_cpp.jsonl")
    py_out = os.path.join(work_dir, "fixture_oracle.jsonl")

    run = subprocess.run([exe, "jsonl", fixture, cpp_out], capture_output=True, text=True)
    if run.returncode != 0:
        fail(f"C++ exited with {run.returncode}: {run.stderr.strip()}")

    with open(fixture, "rb") as f:
        buffer = f.read()
    with open(py_out, "w", newline="\n") as f:
        parsed, skipped = oracle.decode(buffer, specs, f)

    expected_stats = f"parsed: {parsed} skipped: {skipped}"
    if expected_stats not in run.stdout:
        fail(f"stats differ: oracle says '{expected_stats}', C++ printed:\n{run.stdout}")

    cpp_lines, py_lines = read_lines(cpp_out), read_lines(py_out)
    if len(cpp_lines) != len(py_lines):
        fail(f"line count differs: C++ {len(cpp_lines)}, oracle {len(py_lines)}")
    for i, (a, b) in enumerate(zip(cpp_lines, py_lines), start=1):
        if a != b:
            fail(f"line {i} differs:\n  C++:    {a}\n  oracle: {b}")
    print(f"OK: {len(py_lines)} messages identical ({expected_stats})")

    # --- 2. fixture trunchiat ---------------------------------------------
    truncated = os.path.join(fixture_dir, "fixture_truncated.itch")
    run = subprocess.run([exe, "jsonl", truncated, os.path.join(work_dir, "truncated_cpp.jsonl")],
                         capture_output=True, text=True)
    if run.returncode == 0:
        fail("C++ accepted a truncated file")

    with open(truncated, "rb") as f:
        buffer = f.read()
    try:
        with open(os.path.join(work_dir, "truncated_oracle.jsonl"), "w") as f:
            oracle.decode(buffer, specs, f)
        fail("oracle accepted a truncated file")
    except ValueError as e:
        expected_error = str(e)
    if expected_error not in run.stderr:
        fail(f"error differs:\n  oracle: {expected_error}\n  C++:    {run.stderr.strip()}")
    print(f"OK: both report '{expected_error}'")


if __name__ == "__main__":
    main()