//
// Created by Ognean Jason Dennis on 08/09/2026.
//

#ifndef ITCHFEEDHANDLER_HANDLERS_H
#define ITCHFEEDHANDLER_HANDLERS_H
#include <iostream>
#include <fstream>
#include <cstdint>
#include "json_gen.h"


struct CountHandler{
    uint64_t counts[256] = {};
    template <typename M>
    void on(const M&) {
        counts[static_cast<uint8_t>(M::type)]++;
    }
    void print(){
        for(int i{}; i < 256; ++i){
            if(counts[i] == 0) continue;
            std::cout << static_cast<char>(i) << "-" << counts[i] << "\n";
        }
    }
};

struct JsonlHandler{
    std::ofstream json;
    JsonlHandler(const char* file): json(file){}
    template <typename M>
    void on(const M& m) {
        JsonLine j(json);
        itch::write_json(j,m);
        j.end();
    }
};
struct DecodeHandler{

    uint64_t acc{};
    template <typename M>
    void on(const M& m) {
        acc += m.timestamp;
        acc += m.stock_locate;
    }

    void on(const itch::AddOrder& m) {
        acc += m.timestamp;
        acc += m.stock_locate;
        acc += m.price;
        acc += m.shares;
    }

};
#endif //ITCHFEEDHANDLER_HANDLERS_H
