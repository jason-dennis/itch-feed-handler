# ITCH Feed Handler

[![CI](https://github.com/jason-dennis/itch-feed-handler/actions/workflows/ci.yml/badge.svg)](https://github.com/jason-dennis/itch-feed-handler/actions/workflows/ci.yml)

C++17 parser for the NASDAQ TotalView-ITCH 5.0 binary market data feed.

On a full trading day of public NASDAQ BX data (29.2M messages, 863 MB) it
frames and decodes every message in **196 ms — 6.7 ns per message, 149M
messages/s** on one core of an Intel Core i5-12400F (PGO build, median of 10
runs). The output is checked against an independent Python implementation on
all 23 message types, and 18 tests run in CI on Linux, macOS and Windows,
including a build with AddressSanitizer and UndefinedBehaviorSanitizer.

## Results

| Build | Median | Min | ns/msg | M msgs/s |
|---|---|---|---|---|
| Release (`-O3`) | 206.0 ms | 204.4 ms | 7.06 | 142 |
| + `-march=native` | 203.7 ms | 201.5 ms | 6.99 | 143 |
| + PGO | 196.3 ms | 192.0 ms | 6.73 | 149 |

**How this was measured**

- **Data:** [`20191230.BX_ITCH_50`](https://emi.nasdaq.com/ITCH/Nasdaq%20BX%20ITCH/)
  from NASDAQ's public sample server, 862,946,629 bytes, 29,156,757 messages.
- **Machine:** Intel Core i5-12400F, Windows 11, MinGW-w64 GCC 15.2. Process
  not pinned to a core.
- **What is timed:** only `parse()` in `decode` mode — framing, bounds checks,
  dispatch and decoding every field of every message into a typed struct. The
  handler sums a few fields of each message so the decoding can't be optimized
  away. The file is memory-mapped and every page is touched before the timer
  starts, so no I/O or page faults are included.
- **Runs:** one warm-up run, then 10 measured runs with
  [`scripts/bench.py`](scripts/bench.py); median, min and max reported.

**What the numbers say**

- `-march=native` makes no measurable difference (~1%, within run-to-run
  noise). The parser handles one message at a time with data-dependent
  branches, so there is nothing for wider vector instructions to speed up.
- PGO gives a consistent ~4%: even its slowest run is as fast as the median
  without it. The profile was trained on the same file it is measured on, so
  this is a best case; on a day with a different message mix the gain could
  be smaller.

## Correctness

- **Reference implementation.** `python/oracle.py` parses the same format
  independently, straight from the spec file. The end-to-end test runs both on
  a fixture with every message type (random field values, plus an unknown
  type that must be skipped) and requires identical JSONL output, line by line, the
  same parsed/skipped counts, and the same error message on a truncated file.
- **Unit tests** (GoogleTest): big-endian reads, field decoding with distinct
  values in every field, and every malformed-input case — truncated header,
  truncated message, zero length, wrong length for a known type, unknown type.
  Error tests check the exact message, including the byte offset.
- **CI** runs all 18 tests on Linux (GCC), macOS (Clang) and Windows (MSVC),
  plus a GCC build with ASan and UBSan.
- On the real file, per-type message counts are unchanged from the previous
  version of the parser.

## Design

- **Zero-copy input.** The file is memory-mapped (`mmap` on Linux/macOS,
  `MapViewOfFile` on Windows) behind one small RAII class.
- **Bounds-checked framing.** Every read is checked against the remaining
  buffer before it happens; malformed input throws with the byte offset.
  Unknown message types are counted and skipped, so new types in the feed
  don't break the parser.
- **Decode once.** The parser decodes each message into a plain struct with
  native-endian fields (timestamp as `uint64_t`, prices as integers) and calls
  `handler.on(msg)`. Handlers never touch raw bytes.
- **No virtual calls, no allocation in the hot path.** Handlers are template
  parameters; dispatch is a `switch` on the message type.
- **Portable byte-order reads.** Fields are read with shift-and-or helpers;
  GCC and Clang compile them to a single load plus `bswap`/`movbe`, and they
  work on any compiler and any host endianness. No `reinterpret_cast` over the
  input buffer.
- **Generated from the spec.** `data/messagesSpecs.json` describes all 23
  message types. `python/generate.py` turns it into the structs, decoders,
  dispatch switch and JSON writers at build time, and refuses specs where the
  field sizes don't add up to the message length. The Python reference reads
  the same file, so both implementations share one source of truth.

## Build and test

Requires CMake 3.16+, a C++17 compiler and Python 3.

```
cmake -S . -B build              # Release by default
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Options: `-DITCH_NATIVE=OFF` (no `-march=native`), `-DITCH_SANITIZE=ON`
(ASan + UBSan), `-DITCH_BUILD_TESTS=OFF`.

**PGO** (GCC or Clang), in the same build directory:

```
cmake -S . -B build -DITCH_PGO=GENERATE
cmake --build build -j
build/ITCHFeedhandler decode 20191230.BX_ITCH_50    # writes the profile
cmake -S . -B build -DITCH_PGO=USE
cmake --build build -j
```

## Run

```
ITCHFeedhandler count  <file>            messages per type
ITCHFeedhandler decode <file>            full decode, timed
ITCHFeedhandler jsonl  <file> <out>      one JSON object per message
```

Benchmark (10 runs, median/min/max):

```
python scripts/bench.py build/ITCHFeedhandler 20191230.BX_ITCH_50
```

Compare against the Python reference:

```
python tests/compare_with_oracle.py build/ITCHFeedhandler
```

## Message counts (`20191230.BX_ITCH_50`)

| Type | Count | | Type | Count |
|---|---|---|---|---|
| `A` Add Order | 12,210,139 | | `P` Trade | 134,385 |
| `D` Order Delete | 11,821,540 | | `F` Add Order MPID | 45,058 |
| `N` Price Improvement | 2,241,182 | | `Y` Reg SHO | 9,013 |
| `U` Order Replace | 1,741,672 | | `H` Trading Action | 8,961 |
| `E` Order Executed | 578,839 | | `R` Stock Directory | 8,906 |
| `X` Order Cancel | 348,198 | | `L` Market Participant | 6,171 |
| `C` Executed w/ Price | 2,686 | | `S` System Event | 6 |
| | | | `V` MWCB Decline | 1 |

`B`, `I`, `J`, `K`, `O`, `Q`, `W` and `h` are supported but don't appear in
this file. No messages were skipped.

## Structure

```
cpp/include/       parser, handlers, mmap wrapper, byte-order helpers, JSON writer
cpp/src/main.cpp
cpp/generated/     generated at build time (not in git)
python/
  generate.py      spec -> C++ structs, decoders, dispatch, JSON writers
  oracle.py        reference parser
data/messagesSpecs.json
tests/             GoogleTest suite, fixtures, end-to-end comparison
scripts/bench.py
.github/workflows/ci.yml
```

## Limitations

- Replays a file. A production feed handler receives ITCH over UDP multicast
  (MoldUDP64), where network and kernel costs dominate.
- Single-threaded, and it only decodes — there is no order book on top.
- Measures throughput, not per-message latency. At ~7 ns per message, timing
  each message individually would cost more than parsing it.
- Prices are kept as raw integers (`1455000`, not `145.50`) and symbols keep
  their trailing spaces, in both implementations.

Spec: [NASDAQ TotalView-ITCH 5.0](https://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHSpecification.pdf).
