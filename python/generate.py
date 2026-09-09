#!/usr/bin/env python3
"""
Genereaza cele 21 de functii de serializare din messagesSpecs.json.

    python3 gen_writers.py messagesSpecs.json > writers.inc

Numele campurilor din JSON ajung neschimbate in output (contractul cu oracolul),
iar identificatorii de struct se deriva din ele dupa aceeasi regula peste tot.
"""
import json
import sys

# litera -> (nume struct, nume functie)
STRUCTS = {
    'S': ('SystemEvent',               'write_system_event'),
    'R': ('StockDirectory',            'write_stock_directory'),
    'H': ('StockTradingAction',        'write_stock_trading_action'),
    'Y': ('RegSHORestriction',         'write_reg_sho_restriction'),
    'L': ('MarketParticipantPosition', 'write_market_participant_position'),
    'V': ('MWCBDeclineLevel',          'write_mwcb_decline_level'),
    'W': ('MWCBStatus',                'write_mwcb_status'),
    'J': ('LULDAuctionCollar',         'write_luld_auction_collar'),
    'h': ('OperationalHalt',           'write_operational_halt'),
    'A': ('AddOrder',                  'write_add_order'),
    'F': ('AddOrderMPID',              'write_add_order_mpid'),
    'E': ('OrderExecuted',             'write_order_executed'),
    'C': ('OrderExecutedWithPrice',    'write_order_executed_with_price'),
    'X': ('OrderCancel',               'write_order_cancel'),
    'D': ('OrderDelete',               'write_order_delete'),
    'U': ('OrderReplace',              'write_order_replace'),
    'P': ('TradeMessage',              'write_trade_message'),
    'Q': ('CrossTrade',                'write_cross_trade'),
    'B': ('BrokenTrade',               'write_broken_trade'),
    'I': ('NOII',                      'write_noii'),
    'N': ('PriceImprovementIndicator', 'write_price_improvement_indicator'),
}


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
    """'>1s HH 6s 1s' -> [('s',1), ('int',2), ('int',2), ('s',6), ('s',1)]"""
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


def emit(letter, spec):
    struct_name, func_name = STRUCTS[letter]
    toks = tokens(spec['FORMAT'])
    fields = spec['FIELDS']
    assert len(toks) == len(fields), \
        f"{letter}: FORMAT are {len(toks)} campuri, FIELDS are {len(fields)}"
    ts_idx = set(spec.get('UINT48_FIELDS', []))

    lines = [
        f"void {func_name}(const uint8_t* buffer) {{",
        f"    auto* m = reinterpret_cast<const {struct_name}*>(buffer);",
        f"    JsonLine j(json);",
    ]

    # aliniaza numele campurilor ca sa se citeasca coloana
    width = max(len(f) for f in fields) + 2

    for idx, (field, (kind, size)) in enumerate(zip(fields, toks)):
        name = f'"{field}",'.ljust(width + 1)
        member = f"m->{ident(field)}"
        if idx in ts_idx:
            assert kind == 's' and size == 6, f"{letter}: campul {idx} nu e 6s"
            lines.append(f"    j.ts ({name} {member});")
        elif kind == 's' and size == 1:
            lines.append(f"    j.chr({name} {member});")
        elif kind == 's':
            lines.append(f"    j.str({name} {member}, {size});")
        else:
            swap = {2: '__builtin_bswap16', 4: '__builtin_bswap32',
                    8: '__builtin_bswap64'}[size]
            lines.append(f"    j.num({name} {swap}({member}));")

    lines.append("    j.end();")
    lines.append("}")
    return '\n'.join(lines)

def emit_dispatch(specs):
    lines = [
        "void process(uint8_t message_type, const uint8_t* buffer) {",
        "    switch (message_type) {",
    ]
    for letter in specs:
        _, func = STRUCTS[letter]
        lines.append(f"        case '{letter}': {func}(buffer); break;")
    lines.append("        default:")
    lines.append('            throw std::runtime_error(')
    lines.append('                std::string("tip de mesaj netratat in process: ")')
    lines.append('                + static_cast<char>(message_type));')
    lines.append("    }")
    lines.append("}")
    return '\n'.join(lines)

def main():
    path = sys.argv[1] if len(sys.argv) > 1 else '../data/messagesSpecs.json'
    with open(path) as f:
        specs = json.load(f)

    missing = set(specs) - set(STRUCTS)
    if missing:
        sys.exit(f"tipuri in JSON fara struct in generator: {sorted(missing)}")

    print("// Generat de gen_writers.py -- nu edita manual.")
    print("// Regenereaza dupa orice modificare in messagesSpecs.json.\n")
    for letter in specs:
        print(f"// --- {letter} ---")
        print(emit(letter, specs[letter]))
        print()
    print("// --- dispatch ---")
    print(emit_dispatch(specs))


if __name__ == '__main__':
    main()