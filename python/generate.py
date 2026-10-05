#!/usr/bin/env python3
"""
Genereaza codul C++ pentru mesajele ITCH 5.0 din messagesSpecs.json.

    python3 generate.py <messagesSpecs.json> <messages_gen.h> <json_gen.h>

messages_gen.h  -- MESSAGE_LENGTHS, cate un struct decodat per tip (cu decode()),
                   si dispatch(), care decodeaza un mesaj si apeleaza handler.on(...)
json_gen.h      -- cate o functie write_json(JsonLine&, const Struct&) per tip

Numele campurilor din JSON ajung neschimbate in output-ul JSON (contractul cu
oracolul), iar numele membrilor C++ se deriva din ele cu ident(), peste tot la fel.
"""
import json
import sys

HEADER_COMMENT = (
    "// Generat de generate.py din messagesSpecs.json -- nu edita manual.\n"
    "// Se regenereaza automat la build cand se modifica specificatiile.\n"
)


def ident(field_name):
    """'Buy/Sell Indicator' -> 'buy_sell_indicator'"""
    out = []
    for ch in field_name:
        if ch.isalnum():
            out.append(ch.lower())
        else:
            out.append('_')
    s = ''.join(out)
    while '__' in s:
        s = s.replace('__', '_')
    return s.strip('_')


def tokens(fmt):
    """'>1s HH 6s 1s' -> [('s', 1), ('int', 2), ('int', 2), ('s', 6), ('s', 1)]"""
    out = []
    i = 0
    fmt = fmt.replace(' ', '').lstrip('>')
    while i < len(fmt):
        if fmt[i].isdigit():
            n = 0
            while fmt[i].isdigit():
                n = n * 10 + int(fmt[i])
                i += 1
            assert fmt[i] == 's', f"cifra urmata de {fmt[i]!r}, nu de 's'"
            out.append(('s', n))
            i += 1
        else:
            c = fmt[i]
            size = {'H': 2, 'I': 4, 'Q': 8}.get(c)
            assert size, f"tip necunoscut in FORMAT: {c!r}"
            out.append(('int', size))
            i += 1
    return out


def fields_of(letter, spec):
    """
    Lista de campuri (fara Message Type), fiecare ca dict cu:
      name   -- numele original din JSON
      member -- numele membrului C++
      kind   -- 'u48', 'char', 'str' sau 'int'
      size   -- dimensiunea in bytes
      offset -- offset-ul in mesaj (byte-ul de tip e la offset 0)
    Verifica si consistenta specificatiei.
    """
    toks = tokens(spec['FORMAT'])
    names = spec['FIELDS']
    assert len(toks) == len(names), \
        f"{letter}: FORMAT are {len(toks)} campuri, FIELDS are {len(names)}"
    assert toks[0] == ('s', 1) and names[0] == 'Message Type', \
        f"{letter}: primul camp trebuie sa fie Message Type (1s)"

    u48 = set(spec.get('UINT48_FIELDS', []))
    ascii_ = set(spec.get('ASCII_FIELDS', []))

    out = []
    seen = set()
    offset = toks[0][1]
    for idx in range(1, len(toks)):
        kind, size = toks[idx]
        if idx in u48:
            assert (kind, size) == ('s', 6), f"{letter}: campul {idx} e in UINT48_FIELDS dar nu e 6s"
            k = 'u48'
        elif kind == 's':
            assert idx in ascii_, f"{letter}: campul {idx} e 's' dar nu e nici ASCII, nici UINT48"
            k = 'char' if size == 1 else 'str'
        else:
            assert idx not in ascii_, f"{letter}: campul {idx} e numeric dar apare in ASCII_FIELDS"
            k = 'int'
        member = ident(names[idx])
        assert member not in seen, f"{letter}: nume de membru duplicat {member!r}"
        seen.add(member)
        out.append({'name': names[idx], 'member': member, 'kind': k,
                    'size': size, 'offset': offset})
        offset += size

    assert offset == spec['LENGTH'], \
        f"{letter}: suma campurilor e {offset}, dar LENGTH e {spec['LENGTH']}"
    return out


CPP_TYPE = {2: 'uint16_t', 4: 'uint32_t', 8: 'uint64_t'}
READ_FN = {2: 'read_be16', 4: 'read_be32', 8: 'read_be64'}


def cpp_type(f):
    if f['kind'] == 'u48':
        return 'uint64_t'
    if f['kind'] == 'char':
        return 'char'
    if f['kind'] == 'str':
        return f"std::array<char, {f['size']}>"
    return CPP_TYPE[f['size']]


def decode_stmt(f):
    m, off = f['member'], f['offset']
    if f['kind'] == 'u48':
        return f"m.{m} = read_be48(p + {off});"
    if f['kind'] == 'char':
        return f"m.{m} = static_cast<char>(p[{off}]);"
    if f['kind'] == 'str':
        return f"std::memcpy(m.{m}.data(), p + {off}, {f['size']});"
    return f"m.{m} = {READ_FN[f['size']]}(p + {off});"


def emit_lengths(specs):
    table = [0] * 256
    for letter, spec in specs.items():
        table[ord(letter)] = spec['LENGTH']
    lines = ["// Lungimea fiecarui tip de mesaj; 0 = tip necunoscut (sarit de parser).",
             "constexpr uint16_t MESSAGE_LENGTHS[256] = {"]
    for row in range(0, 256, 16):
        vals = ', '.join(f"{v:2d}" for v in table[row:row + 16])
        lines.append(f"    {vals},  // 0x{row:02X}")
    lines.append("};")
    return '\n'.join(lines)


def emit_struct(letter, spec):
    name = spec['NAME']
    fields = fields_of(letter, spec)
    width = max(len(cpp_type(f)) for f in fields)

    lines = [f"// --- {letter}: {name} ---",
             f"struct {name} {{",
             f"    static constexpr char     type   = '{letter}';",
             f"    static constexpr uint16_t length = {spec['LENGTH']};",
             ""]
    for f in fields:
        lines.append(f"    {cpp_type(f).ljust(width)} {f['member']};")
    lines.append("")
    lines.append(f"    static {name} decode(const uint8_t* p) {{")
    lines.append(f"        {name} m;")
    for f in fields:
        lines.append(f"        {decode_stmt(f)}")
    lines.append("        return m;")
    lines.append("    }")
    lines.append("};")
    return '\n'.join(lines)


def emit_dispatch(specs):
    lines = ["// Decodeaza mesajul care incepe la p (byte-ul de tip) si il da handler-ului.",
             "// Tipurile necunoscute nu ajung aici: parser-ul le sare inainte.",
             "template <typename Handler>",
             "inline void dispatch(uint8_t type, const uint8_t* p, Handler& handler) {",
             "    switch (type) {"]
    for letter, spec in specs.items():
        lines.append(f"        case '{letter}': handler.on({spec['NAME']}::decode(p)); return;")
    lines.append("        default: return;")
    lines.append("    }")
    lines.append("}")
    return '\n'.join(lines)


def emit_json(letter, spec):
    name = spec['NAME']
    fields = fields_of(letter, spec)
    width = max(len(f['name']) for f in fields + [{'name': 'Message Type'}]) + 3

    def key(n):
        return f'"{n}",'.ljust(width)

    lines = [f"inline void write_json(JsonLine& j, const {name}& m) {{",
             f"    j.chr({key('Message Type')} {name}::type);"]
    for f in fields:
        if f['kind'] == 'char':
            lines.append(f"    j.chr({key(f['name'])} m.{f['member']});")
        elif f['kind'] == 'str':
            lines.append(f"    j.str({key(f['name'])} m.{f['member']}.data(), {f['size']});")
        else:
            lines.append(f"    j.num({key(f['name'])} m.{f['member']});")
    lines.append("}")
    return '\n'.join(lines)


def messages_header(specs):
    parts = [HEADER_COMMENT,
             "#ifndef ITCH_MESSAGES_GEN_H",
             "#define ITCH_MESSAGES_GEN_H",
             "",
             "#include <array>",
             "#include <cstdint>",
             "#include <cstring>",
             "",
             '#include "byte_order.h"',
             "",
             "namespace itch {",
             "",
             emit_lengths(specs),
             ""]
    for letter, spec in specs.items():
        parts.append(emit_struct(letter, spec))
        parts.append("")
    parts.append(emit_dispatch(specs))
    parts.append("")
    parts.append("}  // namespace itch")
    parts.append("")
    parts.append("#endif  // ITCH_MESSAGES_GEN_H")
    return '\n'.join(parts) + '\n'


def json_header(specs):
    parts = [HEADER_COMMENT,
             "#ifndef ITCH_JSON_GEN_H",
             "#define ITCH_JSON_GEN_H",
             "",
             '#include "jsonwriter.h"',
             '#include "messages_gen.h"',
             "",
             "namespace itch {",
             ""]
    for letter, spec in specs.items():
        parts.append(f"// --- {letter}: {spec['NAME']} ---")
        parts.append(emit_json(letter, spec))
        parts.append("")
    parts.append("}  // namespace itch")
    parts.append("")
    parts.append("#endif  // ITCH_JSON_GEN_H")
    return '\n'.join(parts) + '\n'


def write(path, text):
    with open(path, 'w', newline='\n') as f:
        f.write(text)


def main():
    if len(sys.argv) != 4:
        sys.exit("usage: generate.py <messagesSpecs.json> <messages_gen.h> <json_gen.h>")
    specs_path, messages_path, json_path = sys.argv[1:]

    with open(specs_path) as f:
        specs = json.load(f)

    for letter, spec in specs.items():
        assert len(letter) == 1, f"cheie invalida in JSON: {letter!r}"
        assert 'NAME' in spec, f"{letter}: lipseste NAME"

    write(messages_path, messages_header(specs))
    write(json_path, json_header(specs))


if __name__ == '__main__':
    try:
        main()
    except AssertionError as e:
        sys.exit(f"generate.py: specificatie invalida: {e}")