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
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdio>
int main(int argc, char** argv){

    if(argc != 2){
        throw std::runtime_error("Invalid numbers of arguments");
    }
    int fd = open("sample.BX_ITCH_50", O_RDONLY);
    if(fd < 0){
        throw std::runtime_error("invalid file descriptor");
    }

    struct stat st;
    if(fstat(fd, &st) < 0){
        close(fd);
        throw std::runtime_error("invalid fstat");
    }

    size_t size = st.st_size;
    const uint8_t* buffer = static_cast<const uint8_t*>(mmap(nullptr, size, PROT_READ, MAP_PRIVATE | MAP_POPULATE, fd, 0));
    if(buffer == MAP_FAILED){
        throw std::runtime_error("mmap failed");
    }
    close(fd);

    if(std::string(argv[1]) == "count"){
        CountHandler handler;
        auto before = std::chrono::steady_clock::now();
        parse(buffer, size, handler);
        auto after = std::chrono::steady_clock::now();
        handler.print();
        const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        std::cout << int_ms.count() << " ms\n";
    }
    else if(std::string(argv[1]) == "decode"){
        DecodeHandler handler;
        auto before = std::chrono::steady_clock::now();
        parse(buffer, size, handler);
        auto after = std::chrono::steady_clock::now();
        const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        std::cout << int_ms.count() << " ms\n";
        std::cout<<handler.acc;

    }
    else if(std::string(argv[1]) == "jsonl"){
        JsonlHandler handler("messages_cpp.jsonl");
        auto before = std::chrono::steady_clock::now();
        parse(buffer, size, handler);
        auto after = std::chrono::steady_clock::now();

        const auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(after - before);
        std::cout << int_ms.count() << " ms\n";

    }
    else{
        std::cout<<"Handler invalid";
    }
    munmap((void*)buffer, size);
    return 0;
}