//
// Created by Ognean Jason Dennis on 08/09/2026.
//
#include "handlers.h"
#include "../include/itch_parser.h"
#include <cstdint>
#include <chrono>
#include <string>
#include <iostream>
#include <stdexcept>
#include "mapped_file.h"
int main(int argc, char** argv){
    try {
        if(argc != 3 &&  argc != 4 ){
            throw std::runtime_error("Invalid numbers of arguments");
        }
        MappedFile mapped(argv[2]);
        size_t size = mapped.size();
        const uint8_t* buffer = mapped.data();

        if(std::string(argv[1]) == "count"){
            CountHandler handler;
            auto before = std::chrono::steady_clock::now();
            auto stats =  parse(buffer, size, handler);
            auto after = std::chrono::steady_clock::now();
            handler.print();
            const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
            std::cout << int_ms.count() << " ms\n";
        }
        else if(std::string(argv[1]) == "decode"){
            DecodeHandler handler;
            auto before = std::chrono::steady_clock::now();
            auto stats =  parse(buffer, size, handler);
            auto after = std::chrono::steady_clock::now();
            const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
            std::cout << int_ms.count() << " ms\n";
            std::cout<<handler.acc<<'\n';
            std::cout<<"parsed: "<<stats.parsed <<" skipped: "<<stats.skipped;

        }
        else if(std::string(argv[1]) == "jsonl"){
            if (argc != 4) {
                throw std::runtime_error("Invalid numbers of arguments");
            }
            JsonlHandler handler(argv[3]);
            auto before = std::chrono::steady_clock::now();
            auto stats =  parse(buffer, size, handler);
            auto after = std::chrono::steady_clock::now();

            const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
            std::cout << int_ms.count() << " ms\n";
            std::cout<<"parsed: "<<stats.parsed <<" skipped: "<<stats.skipped;

        }
        else{
            std::cout<<"Handler invalid";
        }
    }
    catch (const std::exception& e){
        std::cerr<<e.what() << '\n';
        return 1;
    }
    return 0;
}