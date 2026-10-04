//
// Created by Ognean Jason Dennis on 08/09/2026.
//

#ifndef ITCHFEEDHANDLER_ITCH_MESSAGES_H
#define ITCHFEEDHANDLER_ITCH_MESSAGES_H
#include <cstdint>
#pragma pack(push, 1)

constexpr uint16_t MESSAGE_LENGTHS[256] {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 19, 36, 19, 31, 40, 0, 25, 50, 35, 0, 26, 0, 20, 0, 44, 40, 39, 12, 0, 35, 35, 12, 23, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};
struct SystemEvent{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char event_code;
};
struct StockDirectory{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char market_category;
    char financial_status_indicator;
    uint32_t round_lot_size;
    char round_lots_only;
    char issue_classification;
    char issue_sub_type[2];
    char authenticity;
    char short_sale_threshold_indicator;
    char ipo_flag;
    char luld_reference_price_tier;
    char etp_flag;
    uint32_t etp_leverage_factor;
    char inverse_indicator;

};

struct StockTradingAction{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char trading_state;
    char reserved;
    char reason[4];
};

struct RegSHORestriction{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char reg_sho_action;
};

struct MarketParticipantPosition{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char mpid[4];
    char stock[8];
    char primary_market_maker;
    char market_maker_mode;
    char market_participant_state;
};

struct MWCBDeclineLevel{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t level_1;
    uint64_t level_2;
    uint64_t level_3;
};

struct MWCBStatus{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char breached_level;
};

struct LULDAuctionCollar{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    uint32_t auction_collar_reference_price;
    uint32_t upper_auction_collar_price;
    uint32_t lower_auction_collar_price;
    uint32_t auction_collar_extension;
};

struct OperationalHalt{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char market_code;
    char operational_halt_action;
};

struct AddOrder{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
};

struct AddOrderMPID{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    char attribution[4];
};

struct OrderExecuted{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
};

struct OrderExecutedWithPrice{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    uint32_t executed_shares;
    uint64_t match_number;
    char printable;
    uint32_t execution_price;
};

struct OrderCancel{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    uint32_t canceled_shares;
};

struct OrderDelete{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
};

struct OrderReplace{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t original_order_reference_number;
    uint64_t new_order_reference_number;
    uint32_t shares;
    uint32_t price;
};

struct TradeMessage{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t order_reference_number;
    char buy_sell_indicator;
    uint32_t shares;
    char stock[8];
    uint32_t price;
    uint64_t match_number;
};

struct CrossTrade{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t shares;
    char stock[8];
    uint32_t cross_price;
    uint64_t match_number;
    char cross_type;
};

struct BrokenTrade{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t match_number;
};

struct NOII{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    uint64_t paired_shares;
    uint64_t imbalance_shares;
    char imbalance_direction;
    char stock[8];
    uint32_t far_price;
    uint32_t near_price;
    uint32_t current_reference_price;
    char cross_type;
    char price_variation_indicator;
};

struct PriceImprovementIndicator{
    char message_type;
    uint16_t stock_locate;
    uint16_t tracking_number;
    uint8_t timestamp[6];
    char stock[8];
    char interest_flag;
};
#pragma pack(pop)
static_assert(sizeof(SystemEvent) == 12);
static_assert(sizeof(StockDirectory) == 39);
static_assert(sizeof(StockTradingAction) == 25);
static_assert(sizeof(RegSHORestriction) == 20);
static_assert(sizeof(MarketParticipantPosition) == 26);
static_assert(sizeof(MWCBDeclineLevel) == 35);
static_assert(sizeof(MWCBStatus) == 12);
static_assert(sizeof(LULDAuctionCollar) == 35);
static_assert(sizeof(OperationalHalt) == 21);
static_assert(sizeof(AddOrder) == 36);
static_assert(sizeof(AddOrderMPID) == 40);
static_assert(sizeof(OrderExecuted) == 31);
static_assert(sizeof(OrderExecutedWithPrice) == 36);
static_assert(sizeof(OrderCancel) == 23);
static_assert(sizeof(OrderDelete) == 19);
static_assert(sizeof(OrderReplace) == 35);
static_assert(sizeof(TradeMessage) == 44);
static_assert(sizeof(CrossTrade) == 40);
static_assert(sizeof(BrokenTrade) == 19);
static_assert(sizeof(NOII) == 50);
static_assert(sizeof(PriceImprovementIndicator) == 20);

#endif //ITCHFEEDHANDLER_ITCH_MESSAGES_H
