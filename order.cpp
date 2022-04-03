#include <stdexcept>
#include <algorithm>

#include "types.h"
#include "price4.h"
#include "ticks.h"
#include "order.h"
#include "engine.h"

MatchingEngine eng;

t_lot lot = eng.lot_size();
TickSizeRule tick_size_rule = eng.tick_size_rule();


void is_valid_price(Price4 price)
{
    // check if an input price satisfies tick size rule
    const auto ticks = tick_size_rule.GetTicks();
    const auto tick_itr = std::lower_bound(ticks.cbegin(), ticks.cend(), price, [](const Tick& lhs, Price4 price){ return lhs.from_price < price;}) - 1;
    const t_tick tick_size = tick_itr->tick_size;
    const t_tick num_ticks = 1.0 * price.unscaled() / 10000 / tick_size;
    if (std::abs(num_ticks - std::round(num_ticks)) > 1e-6)
        throw std::invalid_argument("Order price doesn't match tick size rule!");
}

/// @brief a defualt constructor
Order::Order()
{
    timestamp_ = std::time_t(0);
    order_id_ = 0;
    symbol_ = "NONE";
    quantity_ = 0;
    hiddenqty_ = 0;
    price_ = -1;
    side_ = Side::UNKNOWN;
    type_ = OrderType::UNKNOWN;
    status_ = OrderStatus::UNKNOWN;
    time_in_force_  = TimeInForce::UNKNOWN;
}

/// @brief construct an order by parsing a json object
void Order::init(nlohmann::json& json_order)
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
        status_ = OrderStatus::NEW;
    else if(load == "CANCEL")
    {
        status_ = OrderStatus::CANCEL;
        return; // if it is cancel order, there is no need to try parsing the other information, though the input order line still can contain some extra false informations
    }
    else
        throw std::invalid_argument("Order has a bad type information!");

    
    
    // parse order symbol
    symbol_ = json_order.at("symbol");
    
    
    // parse order type
    try
    {
        load = json_order.at("order_type").get<std::string>();
        if(load == "MARKET")
        {
            type_ = OrderType::MARKET;
            price_ = 0; // if order_type is “MARKET”, limit_price attribute has no significance, we set it as zero.
            
            // parse order quantity
            quantity_ = json_order.at("quantity");
            if(quantity_ <= 0)
                throw std::invalid_argument("Market order has a bad quantity information!");
            if(quantity_ % lot != 0)
                throw std::invalid_argument("Market order quantity is not round lot!");
            
            // parse order side
            load = json_order.at("side").get<std::string>();
            if(load == "BUY")
                side_ = Side::BUY;
            else if(load == "SELL")
                side_ = Side::SELL;
            else
                throw std::invalid_argument("Order has a bad side information!");
            
            return;
        }
        else if(load == "LIMIT")
            type_ = OrderType::LIMIT;
        else if(load == "ICEBERG")
            type_ = OrderType::ICEBERG;
        else
            throw std::invalid_argument("Order has a bad order_type information!");
    }
    catch(const std::exception& err)
    {
        type_ = OrderType::LIMIT; // if it is limit order, the order_type information might be omitted
    }
    
    // parse order price
    load = json_order.at("limit_price").get<std::string>();
    Price4 price_obj(load);
    is_valid_price(price_obj);
    price_ = price_obj.unscaled();
    if(price_ <= 0)
        throw std::invalid_argument("Order has a bad limit price information!");

    // parse order side
    load = json_order.at("side").get<std::string>();
    if(load == "BUY")
        side_ = Side::BUY;
    else if(load == "SELL")
        side_ = Side::SELL;
    else
        throw std::invalid_argument("Order has a bad side information!");

    // parse time-in-force
    try
    {
        load = json_order.at("tif").get<std::string>();
        if(load == "IOC")
            time_in_force_ = TimeInForce::IOC;
        else if(load == "GTC")
            time_in_force_ = TimeInForce::GTC;
        else if(load == "DAY")
            time_in_force_ = TimeInForce::DAY;
        else
            throw std::invalid_argument("Order has a bad time-in-force information!");
    }
    catch(const std::exception& err)
    {
        time_in_force_ = TimeInForce::DAY; // if it is a DAY order, the tif information might be omitted
    }
    
    if(type_ == OrderType::ICEBERG & time_in_force_ == TimeInForce::IOC)
    {
        throw std::invalid_argument("An iceberg order can not be an IOC order!");
        return;
    }
    
    
    if(type_ == OrderType::LIMIT)
    {
        // parse order quantity
        quantity_ = json_order.at("quantity");
        if(quantity_ <= 0)
            throw std::invalid_argument("Order has a bad quantity information!");
        if(quantity_ % lot != 0)
            throw std::invalid_argument("Order quantity is not round lot!");
        return; // if it is limit order, the parsing finishes here
    }
        
    // if it is iceberg order, parse visible/hidden order size
    quantity_ = json_order.at("display");
    if(quantity_ <= 0)
        throw std::invalid_argument("Iceberg order has a bad visible quantity information!");
    if(quantity_ % lot != 0)
        throw std::invalid_argument("Iceberg order quantity is not round lot!");
    
    hiddenqty_ = json_order.at("hidden");
    if(hiddenqty_ <= 0)
        throw std::invalid_argument("Iceberg order has a bad hidden quantity information!");
    if(quantity_ % lot != 0)
        throw std::invalid_argument("Iceberg order hidden quantity is not round lot!");
    
    return;
}
