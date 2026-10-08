// Generat de generate.py din messagesSpecs.json -- nu edita manual.
// Se regenereaza automat la build cand se modifica specificatiile.

#ifndef ITCH_MESSAGES_GEN_H
#define ITCH_MESSAGES_GEN_H

#include <array>
#include <cstdint>
#include <cstring>

#include "byte_order.h"

namespace itch {

// Lungimea fiecarui tip de mesaj; 0 = tip necunoscut (sarit de parser).
constexpr uint16_t MESSAGE_LENGTHS[256] = {
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x00
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x10
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x20
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x30
     0, 36, 19, 36, 19, 31, 40,  0, 25, 50, 35, 28, 26,  0, 20, 48,  // 0x40
    44, 40, 39, 12,  0, 35, 35, 12, 23, 20,  0,  0,  0,  0,  0,  0,  // 0x50
     0,  0,  0,  0,  0,  0,  0,  0, 21,  0,  0,  0,  0,  0,  0,  0,  // 0x60
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x70
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x80
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0x90
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xA0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xB0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xC0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xD0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xE0
     0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  // 0xF0
};

// --- S: SystemEvent ---
struct SystemEvent {
    static constexpr char     type   = 'S';
    static constexpr uint16_t length = 12;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    char     event_code;

    static SystemEvent decode(const uint8_t* p) {
        SystemEvent m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.event_code = static_cast<char>(p[11]);
        return m;
    }
};

// --- R: StockDirectory ---
struct StockDirectory {
    static constexpr char     type   = 'R';
    static constexpr uint16_t length = 39;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    char                market_category;
    char                financial_status_indicator;
    uint32_t            round_lot_size;
    char                round_lots_only;
    char                issue_classification;
    std::array<char, 2> issue_sub_type;
    char                authenticity;
    char                short_sale_threshold_indicator;
    char                ipo_flag;
    char                luld_reference_price_tier;
    char                etp_flag;
    uint32_t            etp_leverage_factor;
    char                inverse_indicator;

    static StockDirectory decode(const uint8_t* p) {
        StockDirectory m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.market_category = static_cast<char>(p[19]);
        m.financial_status_indicator = static_cast<char>(p[20]);
        m.round_lot_size = read_be32(p + 21);
        m.round_lots_only = static_cast<char>(p[25]);
        m.issue_classification = static_cast<char>(p[26]);
        std::memcpy(m.issue_sub_type.data(), p + 27, 2);
        m.authenticity = static_cast<char>(p[29]);
        m.short_sale_threshold_indicator = static_cast<char>(p[30]);
        m.ipo_flag = static_cast<char>(p[31]);
        m.luld_reference_price_tier = static_cast<char>(p[32]);
        m.etp_flag = static_cast<char>(p[33]);
        m.etp_leverage_factor = read_be32(p + 34);
        m.inverse_indicator = static_cast<char>(p[38]);
        return m;
    }
};

// --- H: StockTradingAction ---
struct StockTradingAction {
    static constexpr char     type   = 'H';
    static constexpr uint16_t length = 25;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    char                trading_state;
    char                reserved;
    std::array<char, 4> reason;

    static StockTradingAction decode(const uint8_t* p) {
        StockTradingAction m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.trading_state = static_cast<char>(p[19]);
        m.reserved = static_cast<char>(p[20]);
        std::memcpy(m.reason.data(), p + 21, 4);
        return m;
    }
};

// --- Y: RegSHORestriction ---
struct RegSHORestriction {
    static constexpr char     type   = 'Y';
    static constexpr uint16_t length = 20;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    char                reg_sho_action;

    static RegSHORestriction decode(const uint8_t* p) {
        RegSHORestriction m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.reg_sho_action = static_cast<char>(p[19]);
        return m;
    }
};

// --- L: MarketParticipantPosition ---
struct MarketParticipantPosition {
    static constexpr char     type   = 'L';
    static constexpr uint16_t length = 26;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 4> mpid;
    std::array<char, 8> stock;
    char                primary_market_maker;
    char                market_maker_mode;
    char                market_participant_state;

    static MarketParticipantPosition decode(const uint8_t* p) {
        MarketParticipantPosition m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.mpid.data(), p + 11, 4);
        std::memcpy(m.stock.data(), p + 15, 8);
        m.primary_market_maker = static_cast<char>(p[23]);
        m.market_maker_mode = static_cast<char>(p[24]);
        m.market_participant_state = static_cast<char>(p[25]);
        return m;
    }
};

// --- V: MWCBDeclineLevel ---
struct MWCBDeclineLevel {
    static constexpr char     type   = 'V';
    static constexpr uint16_t length = 35;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t level_1;
    uint64_t level_2;
    uint64_t level_3;

    static MWCBDeclineLevel decode(const uint8_t* p) {
        MWCBDeclineLevel m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.level_1 = read_be64(p + 11);
        m.level_2 = read_be64(p + 19);
        m.level_3 = read_be64(p + 27);
        return m;
    }
};

// --- W: MWCBStatus ---
struct MWCBStatus {
    static constexpr char     type   = 'W';
    static constexpr uint16_t length = 12;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    char     breached_level;

    static MWCBStatus decode(const uint8_t* p) {
        MWCBStatus m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.breached_level = static_cast<char>(p[11]);
        return m;
    }
};

// --- J: LULDAuctionCollar ---
struct LULDAuctionCollar {
    static constexpr char     type   = 'J';
    static constexpr uint16_t length = 35;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    uint32_t            auction_collar_reference_price;
    uint32_t            upper_auction_collar_price;
    uint32_t            lower_auction_collar_price;
    uint32_t            auction_collar_extension;

    static LULDAuctionCollar decode(const uint8_t* p) {
        LULDAuctionCollar m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.auction_collar_reference_price = read_be32(p + 19);
        m.upper_auction_collar_price = read_be32(p + 23);
        m.lower_auction_collar_price = read_be32(p + 27);
        m.auction_collar_extension = read_be32(p + 31);
        return m;
    }
};

// --- h: OperationalHalt ---
struct OperationalHalt {
    static constexpr char     type   = 'h';
    static constexpr uint16_t length = 21;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    char                market_code;
    char                operational_halt_action;

    static OperationalHalt decode(const uint8_t* p) {
        OperationalHalt m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.market_code = static_cast<char>(p[19]);
        m.operational_halt_action = static_cast<char>(p[20]);
        return m;
    }
};

// --- A: AddOrder ---
struct AddOrder {
    static constexpr char     type   = 'A';
    static constexpr uint16_t length = 36;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    uint64_t            order_reference_number;
    char                buy_sell_indicator;
    uint32_t            shares;
    std::array<char, 8> stock;
    uint32_t            price;

    static AddOrder decode(const uint8_t* p) {
        AddOrder m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.order_reference_number = read_be64(p + 11);
        m.buy_sell_indicator = static_cast<char>(p[19]);
        m.shares = read_be32(p + 20);
        std::memcpy(m.stock.data(), p + 24, 8);
        m.price = read_be32(p + 32);
        return m;
    }
};

// --- F: AddOrderMPID ---
struct AddOrderMPID {
    static constexpr char     type   = 'F';
    static constexpr uint16_t length = 40;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    uint64_t            order_reference_number;
    char                buy_sell_indicator;
    uint32_t            shares;
    std::array<char, 8> stock;
    uint32_t            price;
    std::array<char, 4> attribution;

    static AddOrderMPID decode(const uint8_t* p) {
        AddOrderMPID m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.order_reference_number = read_be64(p + 11);
        m.buy_sell_indicator = static_cast<char>(p[19]);
        m.shares = read_be32(p + 20);
        std::memcpy(m.stock.data(), p + 24, 8);
        m.price = read_be32(p + 32);
        std::memcpy(m.attribution.data(), p + 36, 4);
        return m;
    }
};

// --- E: OrderExecuted ---
struct OrderExecuted {
    static constexpr char     type   = 'E';
    static constexpr uint16_t length = 31;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;

    static OrderExecuted decode(const uint8_t* p) {
        OrderExecuted m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.order_reference_number = read_be64(p + 11);
        m.executed_shares = read_be32(p + 19);
        m.match_number = read_be64(p + 23);
        return m;
    }
};

// --- C: OrderExecutedWithPrice ---
struct OrderExecutedWithPrice {
    static constexpr char     type   = 'C';
    static constexpr uint16_t length = 36;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
    char     printable;
    uint32_t execution_price;

    static OrderExecutedWithPrice decode(const uint8_t* p) {
        OrderExecutedWithPrice m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.order_reference_number = read_be64(p + 11);
        m.executed_shares = read_be32(p + 19);
        m.match_number = read_be64(p + 23);
        m.printable = static_cast<char>(p[31]);
        m.execution_price = read_be32(p + 32);
        return m;
    }
};

// --- X: OrderCancel ---
struct OrderCancel {
    static constexpr char     type   = 'X';
    static constexpr uint16_t length = 23;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t order_reference_number;
    uint32_t canceled_shares;

    static OrderCancel decode(const uint8_t* p) {
        OrderCancel m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.order_reference_number = read_be64(p + 11);
        m.canceled_shares = read_be32(p + 19);
        return m;
    }
};

// --- D: OrderDelete ---
struct OrderDelete {
    static constexpr char     type   = 'D';
    static constexpr uint16_t length = 19;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t order_reference_number;

    static OrderDelete decode(const uint8_t* p) {
        OrderDelete m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.order_reference_number = read_be64(p + 11);
        return m;
    }
};

// --- U: OrderReplace ---
struct OrderReplace {
    static constexpr char     type   = 'U';
    static constexpr uint16_t length = 35;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t original_order_reference_number;
    uint64_t new_order_reference_number;
    uint32_t shares;
    uint32_t price;

    static OrderReplace decode(const uint8_t* p) {
        OrderReplace m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.original_order_reference_number = read_be64(p + 11);
        m.new_order_reference_number = read_be64(p + 19);
        m.shares = read_be32(p + 27);
        m.price = read_be32(p + 31);
        return m;
    }
};

// --- P: TradeMessage ---
struct TradeMessage {
    static constexpr char     type   = 'P';
    static constexpr uint16_t length = 44;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    uint64_t            order_reference_number;
    char                buy_sell_indicator;
    uint32_t            shares;
    std::array<char, 8> stock;
    uint32_t            price;
    uint64_t            match_number;

    static TradeMessage decode(const uint8_t* p) {
        TradeMessage m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.order_reference_number = read_be64(p + 11);
        m.buy_sell_indicator = static_cast<char>(p[19]);
        m.shares = read_be32(p + 20);
        std::memcpy(m.stock.data(), p + 24, 8);
        m.price = read_be32(p + 32);
        m.match_number = read_be64(p + 36);
        return m;
    }
};

// --- Q: CrossTrade ---
struct CrossTrade {
    static constexpr char     type   = 'Q';
    static constexpr uint16_t length = 40;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    uint64_t            shares;
    std::array<char, 8> stock;
    uint32_t            cross_price;
    uint64_t            match_number;
    char                cross_type;

    static CrossTrade decode(const uint8_t* p) {
        CrossTrade m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.shares = read_be64(p + 11);
        std::memcpy(m.stock.data(), p + 19, 8);
        m.cross_price = read_be32(p + 27);
        m.match_number = read_be64(p + 31);
        m.cross_type = static_cast<char>(p[39]);
        return m;
    }
};

// --- B: BrokenTrade ---
struct BrokenTrade {
    static constexpr char     type   = 'B';
    static constexpr uint16_t length = 19;

    uint16_t stock_locate;
    uint16_t tracking_number;
    uint64_t timestamp;
    uint64_t match_number;

    static BrokenTrade decode(const uint8_t* p) {
        BrokenTrade m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.match_number = read_be64(p + 11);
        return m;
    }
};

// --- I: NOII ---
struct NOII {
    static constexpr char     type   = 'I';
    static constexpr uint16_t length = 50;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    uint64_t            paired_shares;
    uint64_t            imbalance_shares;
    char                imbalance_direction;
    std::array<char, 8> stock;
    uint32_t            far_price;
    uint32_t            near_price;
    uint32_t            current_reference_price;
    char                cross_type;
    char                price_variation_indicator;

    static NOII decode(const uint8_t* p) {
        NOII m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        m.paired_shares = read_be64(p + 11);
        m.imbalance_shares = read_be64(p + 19);
        m.imbalance_direction = static_cast<char>(p[27]);
        std::memcpy(m.stock.data(), p + 28, 8);
        m.far_price = read_be32(p + 36);
        m.near_price = read_be32(p + 40);
        m.current_reference_price = read_be32(p + 44);
        m.cross_type = static_cast<char>(p[48]);
        m.price_variation_indicator = static_cast<char>(p[49]);
        return m;
    }
};

// --- N: PriceImprovementIndicator ---
struct PriceImprovementIndicator {
    static constexpr char     type   = 'N';
    static constexpr uint16_t length = 20;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    char                interest_flag;

    static PriceImprovementIndicator decode(const uint8_t* p) {
        PriceImprovementIndicator m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.interest_flag = static_cast<char>(p[19]);
        return m;
    }
};

// --- K: IPOQuotingPeriodUpdate ---
struct IPOQuotingPeriodUpdate {
    static constexpr char     type   = 'K';
    static constexpr uint16_t length = 28;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    uint32_t            ipo_quotation_release_time;
    char                ipo_quotation_release_qualifier;
    uint32_t            ipo_price;

    static IPOQuotingPeriodUpdate decode(const uint8_t* p) {
        IPOQuotingPeriodUpdate m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.ipo_quotation_release_time = read_be32(p + 19);
        m.ipo_quotation_release_qualifier = static_cast<char>(p[23]);
        m.ipo_price = read_be32(p + 24);
        return m;
    }
};

// --- O: DirectListingCapitalRaise ---
struct DirectListingCapitalRaise {
    static constexpr char     type   = 'O';
    static constexpr uint16_t length = 48;

    uint16_t            stock_locate;
    uint16_t            tracking_number;
    uint64_t            timestamp;
    std::array<char, 8> stock;
    char                open_eligibility_status;
    uint32_t            minimum_allowable_price;
    uint32_t            maximum_allowable_price;
    uint32_t            near_execution_price;
    uint64_t            near_execution_time;
    uint32_t            lower_price_range_collar;
    uint32_t            upper_price_range_collar;

    static DirectListingCapitalRaise decode(const uint8_t* p) {
        DirectListingCapitalRaise m;
        m.stock_locate = read_be16(p + 1);
        m.tracking_number = read_be16(p + 3);
        m.timestamp = read_be48(p + 5);
        std::memcpy(m.stock.data(), p + 11, 8);
        m.open_eligibility_status = static_cast<char>(p[19]);
        m.minimum_allowable_price = read_be32(p + 20);
        m.maximum_allowable_price = read_be32(p + 24);
        m.near_execution_price = read_be32(p + 28);
        m.near_execution_time = read_be64(p + 32);
        m.lower_price_range_collar = read_be32(p + 40);
        m.upper_price_range_collar = read_be32(p + 44);
        return m;
    }
};

// Decodeaza mesajul care incepe la p (byte-ul de tip) si il da handler-ului.
// Tipurile necunoscute nu ajung aici: parser-ul le sare inainte.
template <typename Handler>
inline void dispatch(uint8_t type, const uint8_t* p, Handler& handler) {
    switch (type) {
        case 'S': handler.on(SystemEvent::decode(p)); return;
        case 'R': handler.on(StockDirectory::decode(p)); return;
        case 'H': handler.on(StockTradingAction::decode(p)); return;
        case 'Y': handler.on(RegSHORestriction::decode(p)); return;
        case 'L': handler.on(MarketParticipantPosition::decode(p)); return;
        case 'V': handler.on(MWCBDeclineLevel::decode(p)); return;
        case 'W': handler.on(MWCBStatus::decode(p)); return;
        case 'J': handler.on(LULDAuctionCollar::decode(p)); return;
        case 'h': handler.on(OperationalHalt::decode(p)); return;
        case 'A': handler.on(AddOrder::decode(p)); return;
        case 'F': handler.on(AddOrderMPID::decode(p)); return;
        case 'E': handler.on(OrderExecuted::decode(p)); return;
        case 'C': handler.on(OrderExecutedWithPrice::decode(p)); return;
        case 'X': handler.on(OrderCancel::decode(p)); return;
        case 'D': handler.on(OrderDelete::decode(p)); return;
        case 'U': handler.on(OrderReplace::decode(p)); return;
        case 'P': handler.on(TradeMessage::decode(p)); return;
        case 'Q': handler.on(CrossTrade::decode(p)); return;
        case 'B': handler.on(BrokenTrade::decode(p)); return;
        case 'I': handler.on(NOII::decode(p)); return;
        case 'N': handler.on(PriceImprovementIndicator::decode(p)); return;
        case 'K': handler.on(IPOQuotingPeriodUpdate::decode(p)); return;
        case 'O': handler.on(DirectListingCapitalRaise::decode(p)); return;
        default: return;
    }
}

}  // namespace itch

#endif  // ITCH_MESSAGES_GEN_H
