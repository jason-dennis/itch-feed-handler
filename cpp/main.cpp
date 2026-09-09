//
// Created by Ognean Jason Dennis on 08/09/2026.
//
#include "itch_messages.h"
#include "itch_parser.h"
#include "handlers.h"
#include <fstream>
#include <cstdint>
#include <vector>
#include <chrono>
#include <string>
int main(int argc, char** argv){

    if(argc != 2){
        exit(1);
    }
    std::ifstream f("sample.BX_ITCH_50", std::ios::binary | std::ios::ate);
    if(!f){
        return 1;
    }
    if(std::string(argv[1]) == "count"){

        size_t size = static_cast<size_t>(f.tellg());
        std::vector<uint8_t> buffer(size);
        f.seekg(0);
        f.read(reinterpret_cast<char*>(buffer.data()), size);

        CountHandler handler;
        auto before = std::chrono::steady_clock::now();
        parse(buffer.data(), size, handler);
        auto after = std::chrono::steady_clock::now();
        handler.print();
        const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        std::cout << int_ms.count() << " ms\n";
    }
    else if(std::string(argv[1]) == "decode"){
        size_t size = static_cast<size_t>(f.tellg());
        std::vector<uint8_t> buffer(size);
        f.seekg(0);
        f.read(reinterpret_cast<char*>(buffer.data()), size);

        DecodeHandler handler;
        auto before = std::chrono::steady_clock::now();
        parse(buffer.data(), size, handler);
        auto after = std::chrono::steady_clock::now();

        const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        std::cout << int_ms.count() << " ms\n";
        std::cout<<handler.acc;

    }
    else{
        size_t size = static_cast<size_t>(f.tellg());
        std::vector<uint8_t> buffer(size);
        f.seekg(0);
        f.read(reinterpret_cast<char*>(buffer.data()), size);

        JsonlHandler handler("messages_cpp.jsonl");
        auto before = std::chrono::steady_clock::now();
        parse(buffer.data(), size, handler);
        auto after = std::chrono::steady_clock::now();

        const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        std::cout << int_ms.count() << " ms\n";

    }


//    handler.print();


    return 0;
}