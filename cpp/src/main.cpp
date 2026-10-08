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
        uint64_t warm = 0;
        for (size_t i = 0; i < size; i += 4096) warm += buffer[i];

        if(std::string(argv[1]) == "count"){
            CountHandler handler;
            auto before = std::chrono::steady_clock::now();
            auto stats =  parse(buffer, size, handler);
            auto after = std::chrono::steady_clock::now();
            handler.print();
            const auto int_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
            const uint64_t total = stats.parsed + stats.skipped;
            std::cout << "time: " << int_ns.count() << " ns, "
                      << (total ? static_cast<double>(int_ns.count()) / total : 0.0) << " ns/msg\n";
            std::cout << "parsed: " << stats.parsed << " skipped: " << stats.skipped << '\n';
        }
        else if(std::string(argv[1]) == "decode"){
            DecodeHandler handler;
            auto before = std::chrono::steady_clock::now();
            auto stats =  parse(buffer, size, handler);
            auto after = std::chrono::steady_clock::now();
            const auto int_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
            const uint64_t total = stats.parsed + stats.skipped;
            std::cout << "acc: " << handler.acc << '\n';
            std::cout << "time: " << int_ns.count() << " ns, "
                      << (total ? static_cast<double>(int_ns.count()) / total : 0.0) << " ns/msg\n";
            std::cout << "parsed: " << stats.parsed << " skipped: " << stats.skipped << '\n';

        }
        else if(std::string(argv[1]) == "jsonl"){
            if (argc != 4) {
                throw std::runtime_error("Invalid numbers of arguments");
            }
            JsonlHandler handler(argv[3]);
            auto before = std::chrono::steady_clock::now();
            auto stats =  parse(buffer, size, handler);
            auto after = std::chrono::steady_clock::now();

            const auto int_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(after - before);
            const uint64_t total = stats.parsed + stats.skipped;
            std::cout << "time: " << int_ns.count() << " ns, "
                      << (total ? static_cast<double>(int_ns.count()) / total : 0.0) << " ns/msg\n";
            std::cout << "parsed: " << stats.parsed << " skipped: " << stats.skipped << '\n';

        }
        else{
            std::cout<<"Handler invalid";
        }
        std::cout << "warmup: " << warm << '\n';
    }
    catch (const std::exception& e){
        std::cerr<<e.what() << '\n';
        return 1;
    }
    return 0;
}