//
// Created by Ognean Jason Dennis on 08/09/2026.
//

#ifndef ITCHFEEDHANDLER_ITCH_PARSER_H
#define ITCHFEEDHANDLER_ITCH_PARSER_H
#include <cstdint>
#include <cstddef>
#include <stdexcept>
#include <sstream>
#include "itch_messages.h"
#include "byte_order.h"

struct ParseStats {
    uint64_t parsed{}, skipped{};
};
template<typename Handler>
ParseStats parse(const uint8_t* buffer, size_t size,Handler& handler){
    ParseStats stats;
    size_t offset{};
    while(offset < size){
        if (size - offset < 2) {
            std::ostringstream msg;
            msg << "truncated header at offset " << offset;
            throw std::runtime_error(msg.str());
        }
        uint16_t message_length = read_be16(buffer + offset);
        if (message_length == 0) {
            std::ostringstream msg;
            msg <<"zero-length message at offset " << offset;
            throw std::runtime_error(msg.str());
        }
        offset += 2;
        if (size - offset < message_length) {
            std::ostringstream msg;
            msg << "truncated message at offset " << offset - 2 << ": need " << message_length
                <<" bytes, have " << size - offset;
            throw std::runtime_error(msg.str());
        }
        uint8_t message_type = buffer[offset];
        if (MESSAGE_LENGTHS[message_type] == 0) {
            stats.skipped ++;
            offset += message_length;
            continue;
        }
        if(MESSAGE_LENGTHS[message_type] != message_length){
            std::ostringstream msg;
            msg << "invalid length for type " << message_type << " at offset " << offset - 2
                <<": expected "<< MESSAGE_LENGTHS[message_type] << ", got " << message_length;
            throw std::runtime_error(msg.str());
        }
        handler.process(message_type, buffer + offset);
        offset += message_length;
        stats.parsed ++;
    }
    return stats;
}
#endif //ITCHFEEDHANDLER_ITCH_PARSER_H
