# NASDAQ TotalView-ITCH 5.0 Feed Handler

A C++ parser for the NASDAQ BX TotalView-ITCH 5.0 binary market data feed,
validated byte-for-byte against a Python reference implementation on a full
trading day of real data.

## Approach

The project is built oracle-first. A spec-driven Python parser was written
before any C++, and its output is the correctness contract: the C++ parser
must produce a byte-identical JSONL file, or it is wrong.

This matters because a binary feed parser fails silently. A field read at the
wrong offset yields a plausible number, not a crash, and the error propagates
through everything downstream. Every optimization in this repo is followed by
a full re-run and `cmp` against the oracle output.

**Python oracle** (`python/oracle.py`) — message layouts live in
`data/messagesSpecs.json`, not in code. One generic decode function is driven
by `FORMAT`, `FIELDS`, `ASCII_FIELDS` and `UINT48_FIELDS` per message type.

**C++ parser** (`cpp/`) — packed structs mapped onto the wire format, one per
message type, each with a `static_assert` on its size against the documented
message length. The framing loop is a template over the handler type, so the
handler inlines and can be swapped without touching the parser.

**Code generation** (`python/generate.py`) — the 21 serialization functions and
the dispatch switch are generated from the same JSON the oracle uses. Field
names in the output cannot drift from the spec, because they are not retyped.

## Validation

| Check | Result |
|---|---|
| Messages parsed | 29,156,757 |
| Input size | 823 MB (`sample.BX_ITCH_50`) |
| Output vs oracle | `cmp` clean, byte-for-byte |
| Struct layouts | 21/21 `static_assert` pass |
| Stream consumed | `offset == file_size` exactly |

Runtime checks in the parser: unknown message type and any mismatch between
the length prefix in the stream and the documented length for that type both
abort immediately. A desynchronized stream produces garbage, not an error, so
it has to be caught at the first byte that does not line up.

### Serialization contract

Three decisions, applied identically in both implementations:

- Symbols are written with their trailing space padding intact — no trimming.
- Prices stay raw integers (`1455000`, not `145.50`). No floats anywhere;
  float formatting differences between Python and C++ would produce diffs that
  are not bugs.
- Field order follows `FIELDS` in the spec JSON.

## Results

<!-- TODO: fill in Mac model, VM vCPU/RAM allocation -->
Measured in a Lima Linux VM on <hardware>. Single core.

| Stage | Handler | Time | msgs/sec |
|---|---|---|---|
| Baseline (`ifstream`, `switch`, `-O2`) | decode | 305 ms | 96M |
| Baseline | count (framing only) | 110 ms | 265M |
| Baseline | JSONL output | 34.6 s | 0.84M |

Three handlers share the same parser:

- `CountHandler` — increments a counter per message type. Upper bound: how
  fast the file can be walked if decoding were free.
- `DecodeHandler` — full decode, every field byte-swapped and accumulated into
  a checksum so the compiler cannot eliminate the reads. **This is the number
  tracked across optimizations.**
- `JsonlHandler` — writes the validation output.

The JSONL number is why the handler is a template parameter rather than code
inlined in the parser: writing the output is ~113x the cost of parsing it, so
any optimization measured through it would be lost in I/O noise.

## Message type coverage

All 21 BX ITCH 5.0 message types are implemented. Six do not occur in the
sample and are therefore **implemented but untested**:

`B` (Broken Trade), `I` (NOII), `J` (LULD Auction Collar), `Q` (Cross Trade),
`W` (MWCB Status), `h` (Operational Halt)

BX does not run opening/closing crosses, which accounts for `Q` and `I`. `J`
is documented as inactive on BX. `W` and `B` require a circuit breaker trip
and a busted trade respectively — neither occurred on this day.

Observed distribution:

| Type | Count | | Type | Count |
|---|---|---|---|---|
| `A` Add Order | 12,210,139 | | `P` Trade | 134,385 |
| `D` Order Delete | 11,821,540 | | `F` Add Order MPID | 45,058 |
| `N` Price Improvement | 2,241,182 | | `Y` Reg SHO | 9,013 |
| `U` Order Replace | 1,741,672 | | `R` Stock Directory | 8,906 |
| `E` Order Executed | 578,839 | | `H` Trading Action | 8,961 |
| `X` Order Cancel | 348,198 | | `L` Market Participant | 6,171 |
| `C` Executed w/ Price | 2,686 | | `S` System Event | 6 |
| | | | `V` MWCB Decline | 1 |

## Building and running

```
g++ -std=c++17 -O2 -o parser cpp/main.cpp

./parser count     # message type counts + framing time
./parser decode    # full decode, timed
./parser jsonl     # writes messages_cpp.jsonl
```

Regenerate the oracle and check:

```
python3 python/oracle.py
cmp messages_cpp.jsonl messages.jsonl
```

Regenerate the serialization code after any spec change:

```
python3 python/generate.py data/messagesSpecs.json > cpp/writers.inc
```

Data files are not in the repo. `sample.BX_ITCH_50` is available from the
NASDAQ ITCH sample data archive.

## Measurement caveats

Throughput was measured inside a virtual machine. Numbers are averaged over
29M messages, so scheduling jitter from the hypervisor averages out and the
relative deltas between optimizations are meaningful.

Tail latency was **not** measured, and would not be meaningful here: the
hypervisor can deschedule a vCPU at any point, so tail percentiles would
describe the VM rather than the parser. That requires bare metal.

Hardware performance counters are unavailable — the PMU is not exposed to the
guest, so `perf stat` reports `<not supported>` for cycles, instructions,
branch misses and cache misses. Only software counters (`page-faults`,
`task-clock`) are usable. Optimizations can therefore be shown to work, but
not explained at the microarchitectural level.

## Not done

- HDR histogram / latency distribution, per-message-type cost
- TSC calibration, sampled instrumentation
- Replayer at original message timestamps; coordinated omission
- Core isolation, `SCHED_FIFO`, C-state control
- Dispatch comparison: `switch` vs function table vs CRTP

The first four require bare metal. The last is portable and is the most
interesting remaining item, since the winner is genuinely not obvious.

## References

NASDAQ BX TotalView-ITCH 5.0 specification, nasdaqtrader.com
