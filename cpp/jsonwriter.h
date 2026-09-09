//
// Created by Ognean Jason Dennis on 09/09/2026.
//

#ifndef ITCHFEEDHANDLER_JSONWRITER_H
#define ITCHFEEDHANDLER_JSONWRITER_H

#include <cstdint>
#include <cstddef>
#include <ostream>

class JsonLine {
    std::ostream& os_;
    bool first_ = true;

    void sep() {
        if (first_) { os_ << '{'; first_ = false; }
        else        { os_ << ", "; }
    }

public:
    explicit JsonLine(std::ostream& os) : os_(os) {}

    // camp numeric: "Nume": 123
    template <typename T>
    void num(const char* name, T value) {
        sep();
        os_ << '"' << name << "\": " << value;
    }

    // camp de un caracter: "Nume": "S"
    void chr(const char* name, char c) {
        sep();
        os_ << '"' << name << "\": \"" << c << '"';
    }

    // camp text de lungime fixa, padding pastrat: "Nume": "AAPL    "
    void str(const char* name, const char* p, size_t n) {
        sep();
        os_ << '"' << name << "\": \"";
        os_.write(p, static_cast<std::streamsize>(n));
        os_ << '"';
    }

    // timestamp pe 6 bytes big-endian
    void ts(const char* name, const uint8_t* p) {
        uint64_t v = 0;
        for (int i = 0; i < 6; ++i) v = (v << 8) | p[i];
        num(name, v);
    }

    void end() { os_ << "}\n"; }
};


#endif //ITCHFEEDHANDLER_JSONWRITER_H
