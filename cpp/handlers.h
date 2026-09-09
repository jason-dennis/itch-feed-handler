//
// Created by Ognean Jason Dennis on 08/09/2026.
//

#ifndef ITCHFEEDHANDLER_HANDLERS_H
#define ITCHFEEDHANDLER_HANDLERS_H
#include <iostream>
#include <fstream>
#include "jsonwriter.h"

struct CountHandler{
    uint64_t counts[256] = {};
    void process(uint8_t message_type, const uint8_t* data){counts[message_type]++;}
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
    // --- S ---
    void write_system_event(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const SystemEvent*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.chr("Event Code",      m->event_code);
        j.end();
    }

// --- R ---
    void write_stock_directory(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const StockDirectory*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",                   m->message_type);
        j.num("Stock Locate",                   __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",                __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",                      m->timestamp);
        j.str("Stock",                          m->stock, 8);
        j.chr("Market Category",                m->market_category);
        j.chr("Financial Status Indicator",     m->financial_status_indicator);
        j.num("Round Lot Size",                 __builtin_bswap32(m->round_lot_size));
        j.chr("Round Lots Only",                m->round_lots_only);
        j.chr("Issue Classification",           m->issue_classification);
        j.str("Issue Sub Type",                 m->issue_sub_type, 2);
        j.chr("Authenticity",                   m->authenticity);
        j.chr("Short Sale Threshold Indicator", m->short_sale_threshold_indicator);
        j.chr("IPO Flag",                       m->ipo_flag);
        j.chr("LULD Reference Price Tier",      m->luld_reference_price_tier);
        j.chr("ETP Flag",                       m->etp_flag);
        j.num("ETP Leverage Factor",            __builtin_bswap32(m->etp_leverage_factor));
        j.chr("Inverse Indicator",              m->inverse_indicator);
        j.end();
    }

// --- H ---
    void write_stock_trading_action(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const StockTradingAction*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.str("Stock",           m->stock, 8);
        j.chr("Trading State",   m->trading_state);
        j.chr("Reserved",        m->reserved);
        j.str("Reason",          m->reason, 4);
        j.end();
    }

// --- Y ---
    void write_reg_sho_restriction(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const RegSHORestriction*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.str("Stock",           m->stock, 8);
        j.chr("Reg SHO Action",  m->reg_sho_action);
        j.end();
    }

// --- L ---
    void write_market_participant_position(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const MarketParticipantPosition*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",             m->message_type);
        j.num("Stock Locate",             __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",          __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",                m->timestamp);
        j.str("MPID",                     m->mpid, 4);
        j.str("Stock",                    m->stock, 8);
        j.chr("Primary Market Maker",     m->primary_market_maker);
        j.chr("Market Maker Mode",        m->market_maker_mode);
        j.chr("Market Participant State", m->market_participant_state);
        j.end();
    }

// --- V ---
    void write_mwcb_decline_level(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const MWCBDeclineLevel*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.num("Level 1",         __builtin_bswap64(m->level_1));
        j.num("Level 2",         __builtin_bswap64(m->level_2));
        j.num("Level 3",         __builtin_bswap64(m->level_3));
        j.end();
    }

// --- W ---
    void write_mwcb_status(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const MWCBStatus*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.chr("Breached Level",  m->breached_level);
        j.end();
    }

// --- J ---
    void write_luld_auction_collar(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const LULDAuctionCollar*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",                   m->message_type);
        j.num("Stock Locate",                   __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",                __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",                      m->timestamp);
        j.str("Stock",                          m->stock, 8);
        j.num("Auction Collar Reference Price", __builtin_bswap32(m->auction_collar_reference_price));
        j.num("Upper Auction Collar Price",     __builtin_bswap32(m->upper_auction_collar_price));
        j.num("Lower Auction Collar Price",     __builtin_bswap32(m->lower_auction_collar_price));
        j.num("Auction Collar Extension",       __builtin_bswap32(m->auction_collar_extension));
        j.end();
    }

// --- h ---
    void write_operational_halt(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const OperationalHalt*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",            m->message_type);
        j.num("Stock Locate",            __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",         __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",               m->timestamp);
        j.str("Stock",                   m->stock, 8);
        j.chr("Market Code",             m->market_code);
        j.chr("Operational Halt Action", m->operational_halt_action);
        j.end();
    }

// --- A ---
    void write_add_order(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const AddOrder*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",           m->message_type);
        j.num("Stock Locate",           __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",        __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",              m->timestamp);
        j.num("Order Reference Number", __builtin_bswap64(m->order_reference_number));
        j.chr("Buy/Sell Indicator",     m->buy_sell_indicator);
        j.num("Shares",                 __builtin_bswap32(m->shares));
        j.str("Stock",                  m->stock, 8);
        j.num("Price",                  __builtin_bswap32(m->price));
        j.end();
    }

// --- F ---
    void write_add_order_mpid(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const AddOrderMPID*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",           m->message_type);
        j.num("Stock Locate",           __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",        __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",              m->timestamp);
        j.num("Order Reference Number", __builtin_bswap64(m->order_reference_number));
        j.chr("Buy/Sell Indicator",     m->buy_sell_indicator);
        j.num("Shares",                 __builtin_bswap32(m->shares));
        j.str("Stock",                  m->stock, 8);
        j.num("Price",                  __builtin_bswap32(m->price));
        j.str("Attribution",            m->attribution, 4);
        j.end();
    }

// --- E ---
    void write_order_executed(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const OrderExecuted*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",           m->message_type);
        j.num("Stock Locate",           __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",        __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",              m->timestamp);
        j.num("Order Reference Number", __builtin_bswap64(m->order_reference_number));
        j.num("Executed Shares",        __builtin_bswap32(m->executed_shares));
        j.num("Match Number",           __builtin_bswap64(m->match_number));
        j.end();
    }

// --- C ---
    void write_order_executed_with_price(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const OrderExecutedWithPrice*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",           m->message_type);
        j.num("Stock Locate",           __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",        __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",              m->timestamp);
        j.num("Order Reference Number", __builtin_bswap64(m->order_reference_number));
        j.num("Executed Shares",        __builtin_bswap32(m->executed_shares));
        j.num("Match Number",           __builtin_bswap64(m->match_number));
        j.chr("Printable",              m->printable);
        j.num("Execution Price",        __builtin_bswap32(m->execution_price));
        j.end();
    }

// --- X ---
    void write_order_cancel(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const OrderCancel*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",           m->message_type);
        j.num("Stock Locate",           __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",        __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",              m->timestamp);
        j.num("Order Reference Number", __builtin_bswap64(m->order_reference_number));
        j.num("Canceled Shares",        __builtin_bswap32(m->canceled_shares));
        j.end();
    }

// --- D ---
    void write_order_delete(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const OrderDelete*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",           m->message_type);
        j.num("Stock Locate",           __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",        __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",              m->timestamp);
        j.num("Order Reference Number", __builtin_bswap64(m->order_reference_number));
        j.end();
    }

// --- U ---
    void write_order_replace(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const OrderReplace*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",                    m->message_type);
        j.num("Stock Locate",                    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",                 __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",                       m->timestamp);
        j.num("Original Order Reference Number", __builtin_bswap64(m->original_order_reference_number));
        j.num("New Order Reference Number",      __builtin_bswap64(m->new_order_reference_number));
        j.num("Shares",                          __builtin_bswap32(m->shares));
        j.num("Price",                           __builtin_bswap32(m->price));
        j.end();
    }

// --- P ---
    void write_trade_message(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const TradeMessage*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",           m->message_type);
        j.num("Stock Locate",           __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",        __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",              m->timestamp);
        j.num("Order Reference Number", __builtin_bswap64(m->order_reference_number));
        j.chr("Buy/Sell Indicator",     m->buy_sell_indicator);
        j.num("Shares",                 __builtin_bswap32(m->shares));
        j.str("Stock",                  m->stock, 8);
        j.num("Price",                  __builtin_bswap32(m->price));
        j.num("Match Number",           __builtin_bswap64(m->match_number));
        j.end();
    }

// --- Q ---
    void write_cross_trade(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const CrossTrade*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.num("Shares",          __builtin_bswap64(m->shares));
        j.str("Stock",           m->stock, 8);
        j.num("Cross Price",     __builtin_bswap32(m->cross_price));
        j.num("Match Number",    __builtin_bswap64(m->match_number));
        j.chr("Cross Type",      m->cross_type);
        j.end();
    }

// --- B ---
    void write_broken_trade(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const BrokenTrade*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.num("Match Number",    __builtin_bswap64(m->match_number));
        j.end();
    }

// --- I ---
    void write_noii(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const NOII*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",              m->message_type);
        j.num("Stock Locate",              __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number",           __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",                 m->timestamp);
        j.num("Paired Shares",             __builtin_bswap64(m->paired_shares));
        j.num("Imbalance Shares",          __builtin_bswap64(m->imbalance_shares));
        j.chr("Imbalance Direction",       m->imbalance_direction);
        j.str("Stock",                     m->stock, 8);
        j.num("Far Price",                 __builtin_bswap32(m->far_price));
        j.num("Near Price",                __builtin_bswap32(m->near_price));
        j.num("Current Reference Price",   __builtin_bswap32(m->current_reference_price));
        j.chr("Cross Type",                m->cross_type);
        j.chr("Price Variation Indicator", m->price_variation_indicator);
        j.end();
    }

// --- N ---
    void write_price_improvement_indicator(const uint8_t* buffer) {
        auto* m = reinterpret_cast<const PriceImprovementIndicator*>(buffer);
        JsonLine j(json);
        j.chr("Message Type",    m->message_type);
        j.num("Stock Locate",    __builtin_bswap16(m->stock_locate));
        j.num("Tracking Number", __builtin_bswap16(m->tracking_number));
        j.ts ("Timestamp",       m->timestamp);
        j.str("Stock",           m->stock, 8);
        j.chr("Interest Flag",   m->interest_flag);
        j.end();
    }

    void process(uint8_t message_type, const uint8_t* buffer) {
        switch (message_type) {
            case 'S': write_system_event(buffer); break;
            case 'R': write_stock_directory(buffer); break;
            case 'H': write_stock_trading_action(buffer); break;
            case 'Y': write_reg_sho_restriction(buffer); break;
            case 'L': write_market_participant_position(buffer); break;
            case 'V': write_mwcb_decline_level(buffer); break;
            case 'W': write_mwcb_status(buffer); break;
            case 'J': write_luld_auction_collar(buffer); break;
            case 'h': write_operational_halt(buffer); break;
            case 'A': write_add_order(buffer); break;
            case 'F': write_add_order_mpid(buffer); break;
            case 'E': write_order_executed(buffer); break;
            case 'C': write_order_executed_with_price(buffer); break;
            case 'X': write_order_cancel(buffer); break;
            case 'D': write_order_delete(buffer); break;
            case 'U': write_order_replace(buffer); break;
            case 'P': write_trade_message(buffer); break;
            case 'Q': write_cross_trade(buffer); break;
            case 'B': write_broken_trade(buffer); break;
            case 'I': write_noii(buffer); break;
            case 'N': write_price_improvement_indicator(buffer); break;
            default:
                throw std::runtime_error(
                        std::string("Unknown Message Type: ")
                        + static_cast<char>(message_type));
        }
    }
};

#endif //ITCHFEEDHANDLER_HANDLERS_H
