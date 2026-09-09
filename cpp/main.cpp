//
// Created by Ognean Jason Dennis on 08/09/2026.
//
#include "itch_messages.h"
#include "itch_parser.h"
#include "handlers.h"
#include <fstream>
#include <cstdint>
#include <vector>

int main(){

    std::ifstream f("sample.BX_ITCH_50", std::ios::binary | std::ios::ate);
    if(!f){
        return 1;
    }
    size_t size = static_cast<size_t>(f.tellg());
    std::vector<uint8_t> buffer(size);
    f.seekg(0);
    f.read(reinterpret_cast<char*>(buffer.data()), size);

    JsonlHandler handler("messagees_cpp.jsonl");
    parse(buffer.data(), size, handler);
//    handler.print();


    return 0;
}