/// @file publisher.h
/// @brief This is a file to implement market data publisher.
/// @author Shangwen Sun
/// @date 04/02/2022
///

#pragma once
#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

#include "types.h"
#include "message.h"

namespace notify
{

#define MAX_MESSAGE_NUM 10000

class Notifier
{
private:
    std::string outfile_; // file path to store the json messages

    std::vector<Callback> bids;
    std::vector<Callback> asks;
    
    std::vector<Callback> messages;
    
public:
    Notifier() = default;
    Notifier(std::string file_name) : outfile_(file_name)
    {
        messages.reserve(MAX_MESSAGE_NUM);
    };
    
    ~Notifier() = default;
    
    /// @brief create a new trade callback
    nlohmann::json notify_trade(lib::t_price price, lib::t_quantity qty);
    
    /// @brief create a new fill/delete/modify callback regarding bid side
    nlohmann::json update_bid(std::string action, lib::t_price price, lib::t_quantity qty);
    
    /// @brief create a new fill/delete/modify callback regarding ask side
    nlohmann::json update_ask(std::string action, lib::t_price price, lib::t_quantity qty);

    nlohmann::json notify_update();
    
    void publish();
    
    void clear();
};

inline nlohmann::json Notifier::notify_trade(lib::t_price price, lib::t_quantity qty)
{
    Callback cb;
    cb.type_ = "TRADE";
    cb.price_ = price;
    cb.qty_ = qty;
    
    messages.emplace_back(cb);
    return cb.to_json();
}

inline nlohmann::json Notifier::update_bid(std::string action, lib::t_price price, lib::t_quantity qty)
{
    Callback cb;
    cb.action_ = action;
    cb.price_ = price;
    cb.qty_ = qty;
    
    bids.emplace_back(cb);
    return cb.to_json();
}

inline nlohmann::json Notifier::update_ask(std::string action, lib::t_price price, lib::t_quantity qty)
{
    Callback cb;
    cb.action_ = action;
    cb.price_ = price;
    cb.qty_ = qty;
    
    asks.emplace_back(cb);
    return cb.to_json();
}


inline nlohmann::json Notifier::notify_update()
{
    Callback cb;
    cb.type_ = "DEPTH_UPDATE";
    cb.bids = bids;
    cb.asks = asks;
    messages.emplace_back(cb);
    return cb.to_json();
}

inline void Notifier::clear()
{
    bids.clear();
    asks.clear();
    messages.clear();
}

inline void Notifier::publish()
{
    // write messages to json file
    std::ofstream file(outfile_);
    for(auto message : messages)
        file << message.to_json();
}

} // namespace notify
