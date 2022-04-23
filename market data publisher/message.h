#pragma once

#include "nlohmann/json.hpp"

#include "types.h"
#include "book.h"
#include "order_entry.h"

namespace notify
{

class Callback
{
public:
    Callback();
    
    nlohmann::json to_json();
    
public:
    std::string type_; // TRADE; DEPTH_UPDATE
    std::string action_; // ADD; DELETE; MODIFY

    lib::t_price price_;
    lib::t_quantity qty_;
    
    std::vector<Callback> bids;
    std::vector<Callback> asks;
};


inline Callback::Callback() : type_("UNKNOWN"), action_("UNKNOWN"), price_(0), qty_(0)
{
    bids.resize(0);
    asks.resize(0);
}

inline nlohmann::json Callback::to_json()
{
    nlohmann::json j;
    j["price"] = std::to_string(1.0 * price_/10000);
    j["quantity"] = qty_;
    
    if(type_ == "UNKNOWN")
    {
        j["action"] = action_;
        return j;
    }
    
    j["type"] = type_;
    
    if(type_ == "TRADE")
        return j;
    
    j["bid"] = nlohmann::json::array();
    j["ask"] = nlohmann::json::array();
    
    for(auto act : bids)
        j["bid"].emplace_back(act.to_json());
    
    for(auto act : asks)
        j["ask"].emplace_back(act.to_json());
    
    return j;
};

} // namespace notify

