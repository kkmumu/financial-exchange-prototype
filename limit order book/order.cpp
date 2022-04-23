#include <stdexcept>
#include <algorithm>

#include "types.h"
#include "price4.h"
#include "ticks.h"
#include "order.h"
#include "engine.h"

using namespace lob;

/// @brief check if an input price satisfies tick size rule
void is_valid_price(lib::TickSizeRule& tsr, lib::Price4 price)
{
    const auto ticks = tsr.GetTicks();
    const auto tick_itr = std::lower_bound(ticks.cbegin(), ticks.cend(), price, [](const lib::Tick& lhs, lib::Price4 price){ return lhs.from_price < price;}) - 1;
    const lib::t_tick tick_size = tick_itr->tick_size;
    const lib::t_tick num_ticks = 1.0 * price.unscaled() / 10000 / tick_size;
    if (std::abs(num_ticks - std::round(num_ticks)) > 1e-6)
        throw std::invalid_argument("Order price doesn't match tick size rule!");
}

/// @brief a defualt constructor
Order::Order(lib::t_time timestamp,
          lib::t_symbol symbol,
          lib::t_orderid order_id,
          lib::t_side is_buy,
          lib::t_price price,
          lib::t_quantity qty,
          lib::OrderStatus status)
          : timestamp_(timestamp),
            symbol_(symbol),
            order_id_(order_id),
            is_buy_(is_buy),
            price_(price),
            open_qty_(qty),
            status_(status) {}

Order::Order(lib::t_time timestamp,
             lib::t_orderid order_id) :
            timestamp_(timestamp),
            order_id_(order_id),
            status_(lib::OrderStatus::CANCEL) {}

/// @brief construct an order by parsing a json object
Order::Order(nlohmann::json& json_order, lib::TickSizeRule& tsr, lib::t_lot lot)
{
    // parse order timestamp
    timestamp_ = json_order.at("time");
    if(timestamp_ <= 0)
        throw std::invalid_argument("Order has a bad timestamp information!");
    
    // parse order id
    order_id_ = json_order.at("order_id");
    if(order_id_ <= 0)
        throw std::invalid_argument("Order has a bad ID information!");
    
    // parse order status
    std::string load = json_order.at("type").get<std::string>();
    if(load == "NEW")
        status_ = lib::OrderStatus::NEW;
    else if(load == "CANCEL")
    {
        status_ = lib::OrderStatus::CANCEL;
        return; // if it is cancel order, there is no need to try parsing the other information, though the input order line still can contain some extra false informations
    }
    else
        throw std::invalid_argument("Order has a bad type information!");

    
    
    // parse order symbol
    symbol_ = json_order.at("symbol").get<std::string>();
    
    
    // parse order type
    try
    {
        load = json_order.at("order_type").get<std::string>();
        if(load == "MARKET")
        {
            type_ = lib::OrderType::MARKET;
            
            // parse order quantity
            order_qty_ = json_order.at("quantity");
            if(order_qty_ <= 0)
                throw std::invalid_argument("Market order has a bad quantity information!");
            if(order_qty_ % lot != 0)
                throw std::invalid_argument("Market order quantity is not round lot!");
            
            // parse order side
            load = json_order.at("side").get<std::string>();
            if(load == "BUY")
            {
                is_buy_ = true;
                price_ = MAX_PRICE;
            }
            else if(load == "SELL")
            {
                is_buy_ = false;
                price_ = MIN_PRICE;
            }
            else
                throw std::invalid_argument("Order has a bad side information!");
            
            return;
        }
        else if(load == "LIMIT")
            type_ = lib::OrderType::LIMIT;
        else if(load == "ICEBERG")
            type_ = lib::OrderType::ICEBERG;
        else
            throw std::invalid_argument("Order has a bad order_type information!");
    }
    catch(const std::exception& err)
    {
        type_ = lib::OrderType::LIMIT; // if it is limit order, the order_type information might be omitted
    }
    
    // parse order price
    load = json_order.at("limit_price").get<std::string>();
    lib::Price4 price_obj(load);
    is_valid_price(tsr, price_obj);
    price_ = price_obj.unscaled();
    if(price_ <= 0)
        throw std::invalid_argument("Order has a bad limit price information!");

    // parse order side
    load = json_order.at("side").get<std::string>();
    if(load == "BUY")
        is_buy_ = 1;
    else if(load == "SELL")
        is_buy_ = 0;
    else
        throw std::invalid_argument("Order has a bad side information!");

    // parse time-in-force
    try
    {
        load = json_order.at("tif").get<std::string>();
        if(load == "IOC")
            condition_ = lib::TimeInForce::IOC;
        else if(load == "GTC")
            condition_ = lib::TimeInForce::GTC;
        else if(load == "DAY")
            condition_ = lib::TimeInForce::DAY;
        else
            throw std::invalid_argument("Order has a bad time-in-force information!");
    }
    catch(const std::exception& err)
    {
        condition_ = lib::TimeInForce::DAY; // if it is a DAY order, the tif information might be omitted
    }
    
    if(type_ == lib::OrderType::ICEBERG & condition_ == lib::TimeInForce::IOC)
    {
        throw std::invalid_argument("An iceberg order can not be an IOC order!");
        return;
    }
    
    
    if(type_ == lib::OrderType::LIMIT)
    {
        // parse order quantity
        order_qty_ = json_order.at("quantity");
        if(order_qty_ <= 0)
            throw std::invalid_argument("Order has a bad quantity information!");
        if(order_qty_ % lot != 0)
            throw std::invalid_argument("Order quantity is not round lot!");
        return; // if it is limit order, the parsing finishes here
    }
        
    // if it is iceberg order, parse visible/hidden order size
    open_qty_ = json_order.at("display");
    if(open_qty_ <= 0)
        throw std::invalid_argument("Iceberg order has a bad visible quantity information!");
    if(open_qty_ % lot != 0)
        throw std::invalid_argument("Iceberg order quantity is not round lot!");
    
    order_qty_ = json_order.at("total");
    if(order_qty_ <= 0)
        throw std::invalid_argument("Iceberg order has a bad hidden quantity information!");
    if(order_qty_ % lot != 0)
        throw std::invalid_argument("Iceberg order hidden quantity is not round lot!");
}


void Order::to_json(nlohmann::json& j)
{
    
    if(status_ == lib::OrderStatus::NEW)
    {
        j = nlohmann::json{{"time", timestamp_},
                            {"type", lib::statusStr[status_]},
                            {"order_id", order_id_},
                            {"symbol", symbol_},
                            {"side", lib::sideStr[is_buy_]},
                            {"quantity", open_qty_},
                            {"limit_price", std::to_string(1.0 * price_/10000)}};
    }
    else
    {
        j = nlohmann::json{{"time", timestamp_},
                            {"type", lib::statusStr[status_]},
                            {"order_id", order_id_}};
    }
    
}
