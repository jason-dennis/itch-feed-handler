//
// Created by denni on 10/4/2026.
//

#ifndef ITCHFEEDHANDLER_BYTE_ORDER_H
#define ITCHFEEDHANDLER_BYTE_ORDER_H
#include <cstdint>
#include <cstddef>
template<typename T, size_t N>
T read_be(const uint8_t* buffer) {
    static_assert(N <= sizeof(T));
   T result{};

    for (size_t byte{}; byte < N; ++byte) {
        result <<= 8;
        result |= buffer[byte];
    }
    return result;
}

inline uint16_t read_be16(const uint8_t* buffer) {
    return read_be<uint16_t, 2>(buffer);
}

inline uint32_t read_be32(const uint8_t* buffer) {
    return read_be<uint32_t, 4>(buffer);
}

inline uint64_t read_be48(const uint8_t* buffer) {
    return read_be<uint64_t, 6>(buffer);
}

inline uint64_t read_be64(const uint8_t* buffer) {
    return read_be<uint64_t, 8>(buffer);
}
#endif //ITCHFEEDHANDLER_BYTE_ORDER_H
