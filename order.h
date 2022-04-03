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


/// @brief implement an order type to be filled in the limit order book
class Order
{
private:
    t_time timestamp_; // time the order arrives
    t_orderid order_id_; // a global unique order id (integer)
    t_symbol symbol_; // the instrument symbol (e.g. AAPL, TSLA)
    t_quantity quantity_; // number of shares to buy or sell
    t_quantity hiddenqty_; // hidden size of an iceberg order
    t_price price_; // price for limit order; 0 for market order
    
    Side side_; // buy or sell
    OrderType type_; // market, limit or iceberg
    OrderStatus status_; // new or cancel
    TimeInForce time_in_force_; // DAY, IOC, GTC
    
public:
    Order();
    void init(nlohmann::json& json_order);
    ~Order() = default;
    
    t_time timestamp() const;
    t_orderid orderid() const;
    t_symbol symbol() const;
    t_quantity quantity() const;
    t_quantity hidden_quantity() const;
    t_price price() const;
    
    Side side() const;
    OrderType order_type() const;
    OrderStatus status() const;
    TimeInForce time_in_force() const;
    
    void set_quantity(const t_quantity qty);

};

inline t_time Order::timestamp() const
{
    return timestamp_;
}

inline t_orderid Order::orderid() const
{
    return order_id_;
}

inline t_symbol Order::symbol() const
{
    return symbol_;
}

inline t_quantity Order::quantity() const
{
    return quantity_;
}

inline t_quantity Order::hidden_quantity() const
{
    return hiddenqty_;
}

inline t_price Order::price() const
{
    return price_;
}

inline Side Order::side() const
{
    return side_;
}

inline OrderType Order::order_type() const
{
    return type_;
}

inline OrderStatus Order::status() const
{
    return status_;
}

inline TimeInForce Order::time_in_force() const
{
    return time_in_force_;
}

inline void Order::set_quantity(const t_quantity qty)
{
    quantity_ = qty;
}
