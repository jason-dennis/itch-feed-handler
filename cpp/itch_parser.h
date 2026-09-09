//
// Created by Ognean Jason Dennis on 08/09/2026.
//

#ifndef ITCHFEEDHANDLER_ITCH_PARSER_H
#define ITCHFEEDHANDLER_ITCH_PARSER_H
#include <cstdint>
#include <cstddef>
#include <stdexcept>
template<typename Handler>
void parse(const uint8_t* buffer, size_t size,Handler& handler){
    size_t offset{};
    while(offset < size){
        uint16_t message_length = buffer[offset];
        message_length <<= 8;
        message_length |= buffer[offset + 1];
        offset += 2;
        uint8_t message_type = buffer[offset];
        if(MESSAGE_LENGTHS[message_type] != message_length){
            throw std::runtime_error("Invalid message length");
        }
        handler.process(message_type, buffer + offset);
        offset += message_length;
    }
    if(offset != size){
        throw std::runtime_error("Invalid parsing");
    }

}
#endif //ITCHFEEDHANDLER_ITCH_PARSER_H
