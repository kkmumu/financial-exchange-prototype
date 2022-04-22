/// @file order.h
/// @brief This is a file to define the Order interface.
/// @author Shangwen Sun
/// @date 03/14/2022

#pragma once

#include <ctime>
#include <string>
#include <fstream>
#include <iostream>
#include <string>

#include "nlohmann/json.hpp"

#include "price4.h"
#include "types.h"
#include "ticks.h"


namespace lob
{

constexpr lib::t_price MAX_PRICE = std::numeric_limits<lib::t_price>::max();
constexpr lib::t_price MIN_PRICE = 1;

/// @brief implement an order type to be filled in the limit order book
class Order
{
public:
    Order(lib::t_time timestamp,
          lib::t_symbol symbol,
          lib::t_orderid order_id,
          lib::t_side is_buy,
          lib::t_price price,
          lib::t_quantity qty,
          lib::OrderStatus status);
    
    Order(lib::t_time timestamp, lib::t_orderid order_id);

    Order(nlohmann::json& json_order, lib::TickSizeRule& tsr, lib::t_lot lot);
    
    lib::t_time timestamp() const;
    
    /// @brief get order id
    lib::t_orderid orderid() const;
    
    /// @brief is this a limit order?
    bool is_limit() const;
    
    /// @brief is this order a buy?
    bool is_buy() const;

    /// @brief get the order's state
    const lib::OrderStatus& status() const;
    
    lib::t_symbol symbol() const;
    
    /// @brief get the limit price of this order
    lib::t_price price() const;

    /// @brief get the total quantity of this order
    lib::t_quantity order_qty() const;

    /// @brief get the dispaly open quantity of this order, if  not iceberg order, equal to order_qty
    lib::t_quantity open_qty() const;
    
    /// @brief get order conditions as a bit mask
    lib::TimeInForce conditions() const;

    /// @brief After generating as many trades as possible against
    /// orders already on the market, cancel any remaining quantity.
    bool immediate_or_cancel() const;
    
    /// @brief if haven't been traded at the end of a trading day, the outstanding portion of a GTC order persists untill next trading day
    /// and will be inserted into the order book based on the ordering that they appear in the previous day’s order book.
    bool good_till_cancel() const;

    /// @brief exchange accepted this order
    void accept();
    
    /// @brief exchange cancelled this order
    void cancel();
    
    /// @brief exchange replaced this order
    /// @param size_delta change to the order quantity
    /// @param new_price the new price
    void replace(lib::t_quantity size_delta, lib::t_price new_price);
    
    void to_json(nlohmann::json& j);

private:
    lib::t_time timestamp_; // time the order arrives
    lib::t_orderid order_id_;
    lib::t_symbol symbol_; // the instrument symbol (e.g. AAPL, TSLA)
    lib::t_quantity open_qty_; // number of shares to display
    lib::t_quantity order_qty_; // number of shares in total
    lib::t_price price_; // price for limit order; 0 for market order
    lib::t_side is_buy_; // 1 for buy, 0 for sell
    
    lib::OrderType type_; // market, limit or iceberg
    lib::OrderStatus status_; // new or cancel
    lib::TimeInForce condition_ = lib::TimeInForce::UNKNOWN; // DAY, IOC, GTC
public:
    static lib::t_orderid self_assigned_id_; // a global unique order id assigned by exchange
};


inline lib::t_time Order::timestamp() const
{
    return timestamp_;
}

inline lib::t_orderid Order::orderid() const
{
    return order_id_;
}

inline bool Order::is_limit() const
{
    return (price_ > 0 && price_ != MAX_PRICE);
}


inline lib::t_side Order::is_buy() const
{
    return is_buy_;
}

inline const lib::OrderStatus& Order::status() const
{
    return status_;
}

inline lib::t_symbol Order::symbol() const
{
    return symbol_;
}

inline lib::t_price Order::price() const
{
    return price_;
}

inline lib::TimeInForce Order::conditions() const
{
    return condition_;
}

inline lib::t_quantity Order::order_qty() const
{
    return order_qty_;
}

inline lib::t_quantity Order::open_qty() const
{
    // If not completely filled, calculate
    return open_qty_;
}

inline bool Order::immediate_or_cancel() const
{
    // market order must be IOC order
    return (condition_ == lib::TimeInForce::IOC) | (!is_limit());
}

inline bool Order::good_till_cancel() const
{
    return (condition_ == lib::TimeInForce::GTC);
}

inline void Order::accept()
{
    if (status_ == lib::OrderStatus::NEW)
        status_ = lib::OrderStatus::ACCEPT;
}

inline void Order::cancel()
{
    if (status_ != lib::OrderStatus::COMPLETE)
        status_ = lib::OrderStatus::CANCEL;
}

inline void Order::replace(lib::t_quantity size_delta, lib::t_price new_price)
{
    if (status_ == lib::OrderStatus::ACCEPT)
    {
        order_qty_ += size_delta;
        price_ = new_price;
    }
}

}

namespace lib
{
typedef lob::Order t_order; // order type alias
}
