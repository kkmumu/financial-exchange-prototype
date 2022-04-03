#pragma once

#include <iostream>

typedef std::time_t t_time;
typedef std::uint64_t t_orderid;
typedef std::string t_symbol;
typedef std::int32_t t_quantity;
typedef std::int64_t t_price;

typedef double t_tick;
typedef std::uint32_t t_lot;

enum class Side
{
    UNKNOWN,
    BUY,
    SELL
};

enum class OrderType
{
    UNKNOWN,
    MARKET,
    LIMIT,
    ICEBERG,
};

enum class OrderStatus
{
    UNKNOWN,
    NEW,
    CANCEL
};


enum class TimeInForce
{
    UNKNOWN,
    DAY, /// @brief A day order rests in the order book until being executed or at the end of a trading day
       ///        At the end of a trading day, day orders will be automatically canceled.
    IOC, /// @brief Immediate-or-cancel.
       ///        The remaining portion of an IOC order will be automatically canceled after matching against the book
    GTC  /// @brief Good-till-cancel. The outstanding portion of a GTC order will persist at the end of a trading day
       ///        The GTC order can be carried day over day, until it is fully traded or canceled.
};

class Order; // forward declaration
typedef Order t_order; // order type alias
