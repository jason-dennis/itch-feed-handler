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
    pos = 0
    while pos < len(buffer):
        message_length = struct.unpack('>H', buffer[pos:pos + 2])[0]
        pos += 2
        message_type = chr(buffer[pos])
        if message_type not in SPECS:
            raise ValueError("Unknown message type")
        if message_length != SPECS[message_type]["LENGTH"]:
            raise ValueError("Invalid message length")
        f.write(json.dumps(decode_message(buffer[pos:pos + message_length], SPECS, message_type)) + '\n')
        pos += message_length
    if pos < len(buffer):
        raise ValueError("Invalid parsing")

def main():
    with open("../data/sample.BX_ITCH_50","rb") as f:
        buffer = f.read()
    with open("../data/messagesSpecs.json","r") as f:
        SPECS = json.load(f)

    with open("../data/messages.jsonl","w") as f:
        decode(buffer, SPECS, f)

if __name__ == "__main__":
    main()

    
    
    

    