//
// Created by denni on 10/8/2026.
//
#include <gtest/gtest.h>
#include "itch_parser.h"


#include <cstdint>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "byte_order.h"
#include "itch_parser.h"
#include "mapped_file.h"
#include "messages_gen.h"

namespace {

// ---------------------------------------------------------------------------
// Unelte pentru construit buffere de test
// ---------------------------------------------------------------------------

// Scrie `value` pe `n` bytes, big-endian, la finalul lui `out`.
void put_be(std::vector<uint8_t>& out, uint64_t value, int n) {
    for (int i = n - 1; i >= 0; --i) {
        out.push_back(static_cast<uint8_t>(value >> (8 * i)));
    }
}

void put_str(std::vector<uint8_t>& out, const std::string& s) {
    out.insert(out.end(), s.begin(), s.end());
}

// Adauga un mesaj complet: header-ul de 2 bytes (lungimea) + corpul.
void append_message(std::vector<uint8_t>& buffer, const std::vector<uint8_t>& body) {
    put_be(buffer, body.size(), 2);
    buffer.insert(buffer.end(), body.begin(), body.end());
}

// Corpul unui Add Order ('A', 36 de bytes), cu valori diferite in fiecare camp.
std::vector<uint8_t> add_order_body() {
    std::vector<uint8_t> b;
    b.push_back('A');
    put_be(b, 0x0102, 2);              // stock locate
    put_be(b, 0x0304, 2);              // tracking number
    put_be(b, 0x0A0B0C0D0E0FULL, 6);   // timestamp (48 de biti)
    put_be(b, 0x1112131415161718ULL, 8); // order reference number
    b.push_back('B');                  // buy/sell
    put_be(b, 0x21222324, 4);          // shares
    put_str(b, "AAPL    ");            // stock
    put_be(b, 0x31323334, 4);          // price
    return b;
}

// Corpul unui System Event ('S', 12 bytes).
std::vector<uint8_t> system_event_body(char event_code) {
    std::vector<uint8_t> b;
    b.push_back('S');
    put_be(b, 0, 2);
    put_be(b, 7, 2);
    put_be(b, 34200000000000ULL, 6);
    b.push_back(static_cast<uint8_t>(event_code));
    return b;
}

// Corpul unui Order Delete ('D', 19 bytes).
std::vector<uint8_t> order_delete_body(uint64_t order_ref) {
    std::vector<uint8_t> b;
    b.push_back('D');
    put_be(b, 1, 2);
    put_be(b, 2, 2);
    put_be(b, 1, 6);
    put_be(b, order_ref, 8);
    return b;
}

// Handler de test: numara mesajele pe tipuri si retine ultimul Add Order.
struct RecordingHandler {
    int counts[256] = {};
    int total = 0;
    itch::AddOrder last_add_order{};

    template <typename M>
    void on(const M&) {
        ++counts[static_cast<uint8_t>(M::type)];
        ++total;
    }

    void on(const itch::AddOrder& m) {
        ++counts[static_cast<uint8_t>('A')];
        ++total;
        last_add_order = m;
    }
};

ParseStats run_parse(const std::vector<uint8_t>& buffer, RecordingHandler& handler) {
    return parse(buffer.data(), buffer.size(), handler);
}

// Verifica ca parse arunca std::runtime_error, iar mesajul contine `expected`.
void expect_parse_error(const std::vector<uint8_t>& buffer, const std::string& expected) {
    RecordingHandler handler;
    try {
        run_parse(buffer, handler);
        ADD_FAILURE() << "parse should have thrown \"" << expected << "\"";
    } catch (const std::runtime_error& e) {
        EXPECT_NE(std::string(e.what()).find(expected), std::string::npos)
            << "actual message: " << e.what();
    }
}

}  // namespace

// ---------------------------------------------------------------------------
// Citire big-endian
// ---------------------------------------------------------------------------

TEST(ByteOrder, ReadsBigEndian) {
    const uint8_t bytes[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    EXPECT_EQ(read_be16(bytes), 0x0102u);
    EXPECT_EQ(read_be32(bytes), 0x01020304u);
    EXPECT_EQ(read_be48(bytes), 0x010203040506ULL);
    EXPECT_EQ(read_be64(bytes), 0x0102030405060708ULL);
}

TEST(ByteOrder, HighBitsAreNotSignExtended) {
    const uint8_t bytes[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    EXPECT_EQ(read_be16(bytes), 0xFFFFu);
    EXPECT_EQ(read_be32(bytes), 0xFFFFFFFFu);
    EXPECT_EQ(read_be48(bytes), 0xFFFFFFFFFFFFULL);
    EXPECT_EQ(read_be64(bytes), 0xFFFFFFFFFFFFFFFFULL);
}

// ---------------------------------------------------------------------------
// Decodare
// ---------------------------------------------------------------------------

TEST(Decode, AddOrderFields) {
    const auto body = add_order_body();
    ASSERT_EQ(body.size(), itch::AddOrder::length);

    const auto m = itch::AddOrder::decode(body.data());
    EXPECT_EQ(m.stock_locate, 0x0102u);
    EXPECT_EQ(m.tracking_number, 0x0304u);
    EXPECT_EQ(m.timestamp, 0x0A0B0C0D0E0FULL);
    EXPECT_EQ(m.order_reference_number, 0x1112131415161718ULL);
    EXPECT_EQ(m.buy_sell_indicator, 'B');
    EXPECT_EQ(m.shares, 0x21222324u);
    EXPECT_EQ(std::string(m.stock.data(), m.stock.size()), "AAPL    ");
    EXPECT_EQ(m.price, 0x31323334u);
}

TEST(Decode, LengthsTableMatchesStructs) {
    EXPECT_EQ(itch::MESSAGE_LENGTHS[static_cast<uint8_t>('A')], itch::AddOrder::length);
    EXPECT_EQ(itch::MESSAGE_LENGTHS[static_cast<uint8_t>('S')], itch::SystemEvent::length);
    EXPECT_EQ(itch::MESSAGE_LENGTHS[static_cast<uint8_t>('K')], 28u);
    EXPECT_EQ(itch::MESSAGE_LENGTHS[static_cast<uint8_t>('O')], 48u);
    EXPECT_EQ(itch::MESSAGE_LENGTHS[static_cast<uint8_t>('Z')], 0u);
}

// ---------------------------------------------------------------------------
// Parser: cazuri valide
// ---------------------------------------------------------------------------

TEST(Parser, EmptyBuffer) {
    RecordingHandler handler;
    const std::vector<uint8_t> buffer;
    const auto stats = run_parse(buffer, handler);
    EXPECT_EQ(stats.parsed, 0u);
    EXPECT_EQ(stats.skipped, 0u);
    EXPECT_EQ(handler.total, 0);
}

TEST(Parser, DecodesAddOrderThroughDispatch) {
    std::vector<uint8_t> buffer;
    append_message(buffer, add_order_body());

    RecordingHandler handler;
    const auto stats = run_parse(buffer, handler);
    EXPECT_EQ(stats.parsed, 1u);
    EXPECT_EQ(handler.counts[static_cast<uint8_t>('A')], 1);
    EXPECT_EQ(handler.last_add_order.price, 0x31323334u);
    EXPECT_EQ(handler.last_add_order.timestamp, 0x0A0B0C0D0E0FULL);
}

TEST(Parser, MultipleMessagesInSequence) {
    std::vector<uint8_t> buffer;
    append_message(buffer, system_event_body('O'));
    append_message(buffer, add_order_body());
    append_message(buffer, order_delete_body(42));
    append_message(buffer, system_event_body('C'));

    RecordingHandler handler;
    const auto stats = run_parse(buffer, handler);
    EXPECT_EQ(stats.parsed, 4u);
    EXPECT_EQ(stats.skipped, 0u);
    EXPECT_EQ(handler.counts[static_cast<uint8_t>('S')], 2);
    EXPECT_EQ(handler.counts[static_cast<uint8_t>('A')], 1);
    EXPECT_EQ(handler.counts[static_cast<uint8_t>('D')], 1);
}

TEST(Parser, UnknownTypeIsSkipped) {
    std::vector<uint8_t> buffer;
    append_message(buffer, {'Z', 1, 2, 3, 4});   // tip necunoscut, 5 bytes
    append_message(buffer, add_order_body());

    RecordingHandler handler;
    const auto stats = run_parse(buffer, handler);
    EXPECT_EQ(stats.skipped, 1u);
    EXPECT_EQ(stats.parsed, 1u);
    EXPECT_EQ(handler.total, 1);
    EXPECT_EQ(handler.counts[static_cast<uint8_t>('Z')], 0);
}

// ---------------------------------------------------------------------------
// Parser: date corupte
// ---------------------------------------------------------------------------

TEST(ParserErrors, TruncatedHeader) {
    expect_parse_error({0x00}, "truncated header at offset 0");
}

TEST(ParserErrors, TruncatedHeaderAfterValidMessage) {
    std::vector<uint8_t> buffer;
    append_message(buffer, system_event_body('O'));   // 14 bytes
    buffer.push_back(0x00);
    expect_parse_error(buffer, "truncated header at offset 14");
}

TEST(ParserErrors, TruncatedMessage) {
    std::vector<uint8_t> buffer;
    append_message(buffer, add_order_body());
    buffer.resize(2 + 10);   // header-ul anunta 36, dar raman doar 10
    expect_parse_error(buffer, "truncated message at offset 0: need 36 bytes, have 10");
}

TEST(ParserErrors, HeaderWithNoBody) {
    expect_parse_error({0x00, 0x24}, "truncated message at offset 0: need 36 bytes, have 0");
}

TEST(ParserErrors, ZeroLengthMessage) {
    expect_parse_error({0x00, 0x00, 'A'}, "zero-length message at offset 0");
}

TEST(ParserErrors, WrongLengthForKnownType) {
    auto body = add_order_body();
    body.pop_back();   // 35 de bytes pentru un 'A'
    std::vector<uint8_t> buffer;
    append_message(buffer, body);
    expect_parse_error(buffer, "invalid length for type A at offset 0: expected 36, got 35");
}

// ---------------------------------------------------------------------------
// MappedFile
// ---------------------------------------------------------------------------

namespace {

std::filesystem::path temp_file(const std::string& name, const std::vector<uint8_t>& content) {
    const auto path = std::filesystem::temp_directory_path() / name;
    std::ofstream out(path, std::ios::binary);
    out.write(reinterpret_cast<const char*>(content.data()),
              static_cast<std::streamsize>(content.size()));
    return path;
}

}  // namespace

TEST(MappedFileTest, EmptyFile) {
    const auto path = temp_file("itch_test_empty.bin", {});
    {
        MappedFile file(path.string().c_str());
        EXPECT_EQ(file.data(), nullptr);
        EXPECT_EQ(file.size(), 0u);
    }
    std::filesystem::remove(path);
}

TEST(MappedFileTest, MapsContent) {
    std::vector<uint8_t> content;
    append_message(content, add_order_body());
    const auto path = temp_file("itch_test_content.bin", content);
    {
        MappedFile file(path.string().c_str());
        ASSERT_EQ(file.size(), content.size());
        EXPECT_TRUE(std::equal(content.begin(), content.end(), file.data()));

        RecordingHandler handler;
        const auto stats = parse(file.data(), file.size(), handler);
        EXPECT_EQ(stats.parsed, 1u);
    }
    std::filesystem::remove(path);
}

TEST(MappedFileTest, MissingFileThrows) {
    const auto path = std::filesystem::temp_directory_path() / "itch_test_does_not_exist.bin";
    std::filesystem::remove(path);
    EXPECT_THROW(MappedFile(path.string().c_str()), std::runtime_error);
}