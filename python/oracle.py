import struct
import json

def decode_message(buffer, SPECS, message_type):
    values = list(struct.unpack(SPECS[message_type]["FORMAT"], buffer))
    for i in SPECS[message_type]["UINT48_FIELDS"]:
        values[i] = int.from_bytes(values[i], 'big')
    for i in SPECS[message_type]["ASCII_FIELDS"]:
        values[i] = values[i].decode('ascii')
    decoded_message = dict(zip(SPECS[message_type]["FIELDS"], values))
    return decoded_message


def decode(buffer, SPECS, f):
    offset = 0
    skipped = 0
    parsed = 0
    while offset < len(buffer):
        if len(buffer) - offset < 2:
            raise ValueError(f"truncated header at offset {offset}")

        message_length = struct.unpack('>H', buffer[offset:offset + 2])[0]
        offset += 2

        if message_length == 0:
            raise ValueError(f"zero-length message at offset {offset - 2}")

        if len(buffer) - offset < message_length:
            raise ValueError(f"truncated message at offset {offset - 2}: need {message_length} bytes, have {len(buffer) - offset}")

        message_type = chr(buffer[offset])

        if message_type not in SPECS:
            skipped += 1
            offset += message_length
            continue

        if message_length != SPECS[message_type]["LENGTH"]:
            raise ValueError(f"invalid length for type {message_type} at offset {offset - 2}: expected {SPECS[message_type]['LENGTH']}, got {message_length}")

        f.write(json.dumps(decode_message(buffer[offset:offset + message_length], SPECS, message_type)) + '\n')
        offset += message_length
        parsed += 1

    return (parsed, skipped)


def main():
    with open("../sample.BX_ITCH_50","rb") as f:
        buffer = f.read()

    with open("../data/messagesSpecs.json","r") as f:
        SPECS = json.load(f)

    with open("../data/messages.jsonl","w") as f:
        parsed, skipped = decode(buffer, SPECS, f)

    print(f"parsed: {parsed} skipped: {skipped}")


if __name__ == "__main__":
    main()