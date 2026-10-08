#!/usr/bin/env python3
"""
Ruleaza ITCHFeedhandler de mai multe ori si raporteaza mediana, minimul si maximul.

    python scripts/bench.py <ITCHFeedhandler> <input.itch> [--mode decode] [--runs 10] [--pin 2]

  --mode   count | decode (implicit: decode)
  --runs   cate rulari masurate (implicit: 10)
  --pin    fixeaza procesul pe un core (doar Linux, cu taskset)

Inainte de rularile masurate se face o rulare de incalzire, care aduce fisierul
in cache-ul sistemului de operare. Programul trebuie sa afiseze linia
"time: <ns> ns, <x> ns/msg" si "parsed: <n> skipped: <m>".
"""
import argparse
import os
import platform
import re
import shutil
import statistics
import subprocess
import sys

TIME_RE = re.compile(r"time:\s*(\d+)\s*ns")
STATS_RE = re.compile(r"parsed:\s*(\d+)\s*skipped:\s*(\d+)")


def cpu_name():
    try:
        if sys.platform.startswith("linux"):
            with open("/proc/cpuinfo") as f:
                for line in f:
                    if line.startswith("model name"):
                        return line.split(":", 1)[1].strip()
        elif sys.platform == "darwin":
            return subprocess.run(["sysctl", "-n", "machdep.cpu.brand_string"],
                                  capture_output=True, text=True).stdout.strip()
        elif sys.platform == "win32":
            out = subprocess.run(["powershell", "-NoProfile", "-Command",
                                  "(Get-CimInstance Win32_Processor).Name"],
                                 capture_output=True, text=True).stdout.strip()
            if out:
                return out
    except OSError:
        pass
    return platform.processor() or "unknown"


def run_once(cmd):
    run = subprocess.run(cmd, capture_output=True, text=True)
    if run.returncode != 0:
        sys.exit(f"command failed ({run.returncode}): {' '.join(cmd)}\n{run.stderr.strip()}")
    t = TIME_RE.search(run.stdout)
    s = STATS_RE.search(run.stdout)
    if not t or not s:
        sys.exit(f"could not find 'time:' / 'parsed:' in output:\n{run.stdout}")
    return int(t.group(1)), int(s.group(1)) + int(s.group(2))


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("exe")
    ap.add_argument("input")
    ap.add_argument("--mode", default="decode", choices=["count", "decode"])
    ap.add_argument("--runs", type=int, default=10)
    ap.add_argument("--pin", type=int, default=None, help="core to pin to (Linux only)")
    args = ap.parse_args()

    cmd = [os.path.abspath(args.exe), args.mode, args.input]
    if args.pin is not None:
        if not sys.platform.startswith("linux") or not shutil.which("taskset"):
            sys.exit("--pin needs Linux with taskset")
        cmd = ["taskset", "-c", str(args.pin)] + cmd

    print(f"CPU:   {cpu_name()}")
    print(f"OS:    {platform.system()} {platform.release()}")
    print(f"Cmd:   {' '.join(cmd)}")
    print(f"Input: {args.input} ({os.path.getsize(args.input):,} bytes)")

    run_once(cmd)  # incalzire: aduce fisierul in cache-ul sistemului

    times = []
    messages = 0
    for i in range(args.runs):
        ns, messages = run_once(cmd)
        times.append(ns)
        print(f"  run {i + 1:2d}: {ns / 1e6:9.2f} ms  {ns / messages:6.2f} ns/msg")

    med, lo, hi = statistics.median(times), min(times), max(times)
    print()
    print(f"Messages: {messages:,}")
    print(f"Median:   {med / 1e6:.2f} ms  ({med / messages:.2f} ns/msg, {messages / med * 1e3:.1f} M msgs/s)")
    print(f"Min:      {lo / 1e6:.2f} ms  ({lo / messages:.2f} ns/msg)")
    print(f"Max:      {hi / 1e6:.2f} ms  ({hi / messages:.2f} ns/msg)")
    print()
    print("Markdown row (median | min | ns/msg | M msgs/s):")
    print(f"| {med / 1e6:.1f} ms | {lo / 1e6:.1f} ms | {med / messages:.2f} | {messages / med * 1e3:.0f} |")


if __name__ == "__main__":
    main()