#pragma once

#include <iostream>
#include <chrono>
#include <map>

namespace lib
{

typedef std::time_t t_time;
typedef std::uint64_t t_orderid;
typedef std::int32_t t_quantity;
typedef std::int64_t t_price;

typedef double t_tick;
typedef std::uint32_t t_lot;
typedef bool t_side;

typedef std::string FILE;

enum class OrderType
{
    UNKNOWN = 0,
    LIMIT = 1,
    MARKET = 2,
    ICEBERG = 3,
};

enum class OrderStatus
{
    UNKNOWN = 0,
    NEW = 1,
    CANCEL = 2,
    COMPLETE = 3,
    ACCEPT = 4,
    REJECT = 5,
};
static std::map<OrderStatus, std::string> statusStr {{OrderStatus::NEW, "NEW"}, {OrderStatus::CANCEL, "CANCEL"}};

static std::map<bool, std::string> sideStr {{true, "BUY"}, {false, "SELL"}};

enum class TimeInForce
{
    UNKNOWN = 0,
    DAY = 1, /// @brief A day order rests in the order book until being executed or at the end of a trading day
       ///        At the end of a trading day, day orders will be automatically canceled.
    IOC = 2, /// @brief Immediate-or-cancel.
       ///        The remaining portion of an IOC order will be automatically canceled after matching against the book
    GTC = 3  /// @brief Good-till-cancel. The outstanding portion of a GTC order will persist at the end of a trading day
       ///        The GTC order can be carried day over day, until it is fully traded or canceled.
};

//enum class Symbol
//{
//    UNKNOWN,
//    AAPL,
//    MSFT,
//    NVDA,
//    AVGO,
//    CSCO
//};
typedef std::string t_symbol;
//static std::map<Symbol, std::string> symbolStr{
//                                {Symbol::AAPL, "AAPL"},
//                                {Symbol::MSFT, "MSFT"},
//                                {Symbol::NVDA, "NVDA"},
//                                {Symbol::AVGO, "AVGO"},
//                                {Symbol::CSCO, "CSCO"}};
//static std::map<std::string, Symbol> symbolStr_reverse{
//                                {"AAPL", Symbol::AAPL},
//                                {"MSFT", Symbol::MSFT},
//                                {"NVDA", Symbol::NVDA},
//                                {"AVGO", Symbol::AVGO},
//                                {"CSCO", Symbol::CSCO}};

}


