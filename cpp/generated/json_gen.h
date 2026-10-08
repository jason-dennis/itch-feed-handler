// Generat de generate.py din messagesSpecs.json -- nu edita manual.
// Se regenereaza automat la build cand se modifica specificatiile.

#ifndef ITCH_JSON_GEN_H
#define ITCH_JSON_GEN_H

#include "jsonwriter.h"
#include "messages_gen.h"

namespace itch {

// --- S: SystemEvent ---
inline void write_json(JsonLine& j, const SystemEvent& m) {
    j.chr("Message Type",    SystemEvent::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.chr("Event Code",      m.event_code);
}

// --- R: StockDirectory ---
inline void write_json(JsonLine& j, const StockDirectory& m) {
    j.chr("Message Type",                   StockDirectory::type);
    j.num("Stock Locate",                   m.stock_locate);
    j.num("Tracking Number",                m.tracking_number);
    j.num("Timestamp",                      m.timestamp);
    j.str("Stock",                          m.stock.data(), 8);
    j.chr("Market Category",                m.market_category);
    j.chr("Financial Status Indicator",     m.financial_status_indicator);
    j.num("Round Lot Size",                 m.round_lot_size);
    j.chr("Round Lots Only",                m.round_lots_only);
    j.chr("Issue Classification",           m.issue_classification);
    j.str("Issue Sub Type",                 m.issue_sub_type.data(), 2);
    j.chr("Authenticity",                   m.authenticity);
    j.chr("Short Sale Threshold Indicator", m.short_sale_threshold_indicator);
    j.chr("IPO Flag",                       m.ipo_flag);
    j.chr("LULD Reference Price Tier",      m.luld_reference_price_tier);
    j.chr("ETP Flag",                       m.etp_flag);
    j.num("ETP Leverage Factor",            m.etp_leverage_factor);
    j.chr("Inverse Indicator",              m.inverse_indicator);
}

// --- H: StockTradingAction ---
inline void write_json(JsonLine& j, const StockTradingAction& m) {
    j.chr("Message Type",    StockTradingAction::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.str("Stock",           m.stock.data(), 8);
    j.chr("Trading State",   m.trading_state);
    j.chr("Reserved",        m.reserved);
    j.str("Reason",          m.reason.data(), 4);
}

// --- Y: RegSHORestriction ---
inline void write_json(JsonLine& j, const RegSHORestriction& m) {
    j.chr("Message Type",    RegSHORestriction::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.str("Stock",           m.stock.data(), 8);
    j.chr("Reg SHO Action",  m.reg_sho_action);
}

// --- L: MarketParticipantPosition ---
inline void write_json(JsonLine& j, const MarketParticipantPosition& m) {
    j.chr("Message Type",             MarketParticipantPosition::type);
    j.num("Stock Locate",             m.stock_locate);
    j.num("Tracking Number",          m.tracking_number);
    j.num("Timestamp",                m.timestamp);
    j.str("MPID",                     m.mpid.data(), 4);
    j.str("Stock",                    m.stock.data(), 8);
    j.chr("Primary Market Maker",     m.primary_market_maker);
    j.chr("Market Maker Mode",        m.market_maker_mode);
    j.chr("Market Participant State", m.market_participant_state);
}

// --- V: MWCBDeclineLevel ---
inline void write_json(JsonLine& j, const MWCBDeclineLevel& m) {
    j.chr("Message Type",    MWCBDeclineLevel::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.num("Level 1",         m.level_1);
    j.num("Level 2",         m.level_2);
    j.num("Level 3",         m.level_3);
}

// --- W: MWCBStatus ---
inline void write_json(JsonLine& j, const MWCBStatus& m) {
    j.chr("Message Type",    MWCBStatus::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.chr("Breached Level",  m.breached_level);
}

// --- J: LULDAuctionCollar ---
inline void write_json(JsonLine& j, const LULDAuctionCollar& m) {
    j.chr("Message Type",                   LULDAuctionCollar::type);
    j.num("Stock Locate",                   m.stock_locate);
    j.num("Tracking Number",                m.tracking_number);
    j.num("Timestamp",                      m.timestamp);
    j.str("Stock",                          m.stock.data(), 8);
    j.num("Auction Collar Reference Price", m.auction_collar_reference_price);
    j.num("Upper Auction Collar Price",     m.upper_auction_collar_price);
    j.num("Lower Auction Collar Price",     m.lower_auction_collar_price);
    j.num("Auction Collar Extension",       m.auction_collar_extension);
}

// --- h: OperationalHalt ---
inline void write_json(JsonLine& j, const OperationalHalt& m) {
    j.chr("Message Type",            OperationalHalt::type);
    j.num("Stock Locate",            m.stock_locate);
    j.num("Tracking Number",         m.tracking_number);
    j.num("Timestamp",               m.timestamp);
    j.str("Stock",                   m.stock.data(), 8);
    j.chr("Market Code",             m.market_code);
    j.chr("Operational Halt Action", m.operational_halt_action);
}

// --- A: AddOrder ---
inline void write_json(JsonLine& j, const AddOrder& m) {
    j.chr("Message Type",           AddOrder::type);
    j.num("Stock Locate",           m.stock_locate);
    j.num("Tracking Number",        m.tracking_number);
    j.num("Timestamp",              m.timestamp);
    j.num("Order Reference Number", m.order_reference_number);
    j.chr("Buy/Sell Indicator",     m.buy_sell_indicator);
    j.num("Shares",                 m.shares);
    j.str("Stock",                  m.stock.data(), 8);
    j.num("Price",                  m.price);
}

// --- F: AddOrderMPID ---
inline void write_json(JsonLine& j, const AddOrderMPID& m) {
    j.chr("Message Type",           AddOrderMPID::type);
    j.num("Stock Locate",           m.stock_locate);
    j.num("Tracking Number",        m.tracking_number);
    j.num("Timestamp",              m.timestamp);
    j.num("Order Reference Number", m.order_reference_number);
    j.chr("Buy/Sell Indicator",     m.buy_sell_indicator);
    j.num("Shares",                 m.shares);
    j.str("Stock",                  m.stock.data(), 8);
    j.num("Price",                  m.price);
    j.str("Attribution",            m.attribution.data(), 4);
}

// --- E: OrderExecuted ---
inline void write_json(JsonLine& j, const OrderExecuted& m) {
    j.chr("Message Type",           OrderExecuted::type);
    j.num("Stock Locate",           m.stock_locate);
    j.num("Tracking Number",        m.tracking_number);
    j.num("Timestamp",              m.timestamp);
    j.num("Order Reference Number", m.order_reference_number);
    j.num("Executed Shares",        m.executed_shares);
    j.num("Match Number",           m.match_number);
}

// --- C: OrderExecutedWithPrice ---
inline void write_json(JsonLine& j, const OrderExecutedWithPrice& m) {
    j.chr("Message Type",           OrderExecutedWithPrice::type);
    j.num("Stock Locate",           m.stock_locate);
    j.num("Tracking Number",        m.tracking_number);
    j.num("Timestamp",              m.timestamp);
    j.num("Order Reference Number", m.order_reference_number);
    j.num("Executed Shares",        m.executed_shares);
    j.num("Match Number",           m.match_number);
    j.chr("Printable",              m.printable);
    j.num("Execution Price",        m.execution_price);
}

// --- X: OrderCancel ---
inline void write_json(JsonLine& j, const OrderCancel& m) {
    j.chr("Message Type",           OrderCancel::type);
    j.num("Stock Locate",           m.stock_locate);
    j.num("Tracking Number",        m.tracking_number);
    j.num("Timestamp",              m.timestamp);
    j.num("Order Reference Number", m.order_reference_number);
    j.num("Canceled Shares",        m.canceled_shares);
}

// --- D: OrderDelete ---
inline void write_json(JsonLine& j, const OrderDelete& m) {
    j.chr("Message Type",           OrderDelete::type);
    j.num("Stock Locate",           m.stock_locate);
    j.num("Tracking Number",        m.tracking_number);
    j.num("Timestamp",              m.timestamp);
    j.num("Order Reference Number", m.order_reference_number);
}

// --- U: OrderReplace ---
inline void write_json(JsonLine& j, const OrderReplace& m) {
    j.chr("Message Type",                    OrderReplace::type);
    j.num("Stock Locate",                    m.stock_locate);
    j.num("Tracking Number",                 m.tracking_number);
    j.num("Timestamp",                       m.timestamp);
    j.num("Original Order Reference Number", m.original_order_reference_number);
    j.num("New Order Reference Number",      m.new_order_reference_number);
    j.num("Shares",                          m.shares);
    j.num("Price",                           m.price);
}

// --- P: TradeMessage ---
inline void write_json(JsonLine& j, const TradeMessage& m) {
    j.chr("Message Type",           TradeMessage::type);
    j.num("Stock Locate",           m.stock_locate);
    j.num("Tracking Number",        m.tracking_number);
    j.num("Timestamp",              m.timestamp);
    j.num("Order Reference Number", m.order_reference_number);
    j.chr("Buy/Sell Indicator",     m.buy_sell_indicator);
    j.num("Shares",                 m.shares);
    j.str("Stock",                  m.stock.data(), 8);
    j.num("Price",                  m.price);
    j.num("Match Number",           m.match_number);
}

// --- Q: CrossTrade ---
inline void write_json(JsonLine& j, const CrossTrade& m) {
    j.chr("Message Type",    CrossTrade::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.num("Shares",          m.shares);
    j.str("Stock",           m.stock.data(), 8);
    j.num("Cross Price",     m.cross_price);
    j.num("Match Number",    m.match_number);
    j.chr("Cross Type",      m.cross_type);
}

// --- B: BrokenTrade ---
inline void write_json(JsonLine& j, const BrokenTrade& m) {
    j.chr("Message Type",    BrokenTrade::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.num("Match Number",    m.match_number);
}

// --- I: NOII ---
inline void write_json(JsonLine& j, const NOII& m) {
    j.chr("Message Type",              NOII::type);
    j.num("Stock Locate",              m.stock_locate);
    j.num("Tracking Number",           m.tracking_number);
    j.num("Timestamp",                 m.timestamp);
    j.num("Paired Shares",             m.paired_shares);
    j.num("Imbalance Shares",          m.imbalance_shares);
    j.chr("Imbalance Direction",       m.imbalance_direction);
    j.str("Stock",                     m.stock.data(), 8);
    j.num("Far Price",                 m.far_price);
    j.num("Near Price",                m.near_price);
    j.num("Current Reference Price",   m.current_reference_price);
    j.chr("Cross Type",                m.cross_type);
    j.chr("Price Variation Indicator", m.price_variation_indicator);
}

// --- N: PriceImprovementIndicator ---
inline void write_json(JsonLine& j, const PriceImprovementIndicator& m) {
    j.chr("Message Type",    PriceImprovementIndicator::type);
    j.num("Stock Locate",    m.stock_locate);
    j.num("Tracking Number", m.tracking_number);
    j.num("Timestamp",       m.timestamp);
    j.str("Stock",           m.stock.data(), 8);
    j.chr("Interest Flag",   m.interest_flag);
}

// --- K: IPOQuotingPeriodUpdate ---
inline void write_json(JsonLine& j, const IPOQuotingPeriodUpdate& m) {
    j.chr("Message Type",                    IPOQuotingPeriodUpdate::type);
    j.num("Stock Locate",                    m.stock_locate);
    j.num("Tracking Number",                 m.tracking_number);
    j.num("Timestamp",                       m.timestamp);
    j.str("Stock",                           m.stock.data(), 8);
    j.num("IPO Quotation Release Time",      m.ipo_quotation_release_time);
    j.chr("IPO Quotation Release Qualifier", m.ipo_quotation_release_qualifier);
    j.num("IPO Price",                       m.ipo_price);
}

// --- O: DirectListingCapitalRaise ---
inline void write_json(JsonLine& j, const DirectListingCapitalRaise& m) {
    j.chr("Message Type",             DirectListingCapitalRaise::type);
    j.num("Stock Locate",             m.stock_locate);
    j.num("Tracking Number",          m.tracking_number);
    j.num("Timestamp",                m.timestamp);
    j.str("Stock",                    m.stock.data(), 8);
    j.chr("Open Eligibility Status",  m.open_eligibility_status);
    j.num("Minimum Allowable Price",  m.minimum_allowable_price);
    j.num("Maximum Allowable Price",  m.maximum_allowable_price);
    j.num("Near Execution Price",     m.near_execution_price);
    j.num("Near Execution Time",      m.near_execution_time);
    j.num("Lower Price Range Collar", m.lower_price_range_collar);
    j.num("Upper Price Range Collar", m.upper_price_range_collar);
}

}  // namespace itch

#endif  // ITCH_JSON_GEN_H
