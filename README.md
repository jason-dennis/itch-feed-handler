# ITCH Feed Handler

C++ parser for the NASDAQ BX TotalView-ITCH 5.0 binary market data feed.

Includes a Python implementation used as a reference. Both parse the same file
and produce the same JSONL output, so the C++ version can be checked with
`cmp` after every change.

## Structure

```
cpp/
  itch_messages.h   packed structs, one per message type
  itch_parser.h     framing loop, template over the handler
  handlers.h        count / decode / jsonl handlers
  jsonwriter.h      JSON line writer
  main.cpp
python/
  oracle.py         reference parser
  generate.py       generates the C++ serialization from the spec
data/
  messagesSpecs.json   message layouts
```

Message layouts are in `messagesSpecs.json`. The Python parser reads it at
runtime; the C++ serialization functions are generated from it. Each struct
has a `static_assert` checking its size against the documented message length.

## Build

```
g++ -std=c++17 -O3 -march=native -o parser cpp/main.cpp
```

## Run

```
./parser count     message counts per type
./parser decode    full decode, timed
./parser jsonl     writes messages_cpp.jsonl
```

Check against the reference:

```
python3 python/oracle.py
cmp messages_cpp.jsonl messages.jsonl
```

Regenerate the serialization code after changing the spec:

```
python3 python/generate.py data/messagesSpecs.json > cpp/writers.inc
```

The data file is not in the repo — `sample.BX_ITCH_50` is from the NASDAQ ITCH
sample archive.

## Results

823 MB file, 29,156,757 messages. `cmp` against the Python output is clean.

Timings are for `decode`, median of 3 runs, in a Linux VM on <hardware>:

| Build | Time | msgs/sec |
|---|---|---|
| `ifstream`, `-O2` | 305 ms | 96M |
| `mmap` + `MAP_POPULATE` | 323 ms | 90M |
| `-O3 -march=native` | 244 ms | 119M |
| PGO | 222 ms | 131M |

`mmap` didn't change the time (only `parse` is timed, and the read happens
before that) but page faults went from ~200k to 118.

For reference: framing alone is 110 ms (265M msgs/sec), and writing the JSONL
output takes 34.6 s.

## Message counts

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

`B`, `I`, `J`, `Q`, `W` and `h` are implemented but don't appear in this file.

## Notes

Prices are kept as raw integers (`1455000`, not `145.50`) and symbols keep
their trailing spaces, in both implementations.

Spec: NASDAQ BX TotalView-ITCH 5.0, nasdaqtrader.com
