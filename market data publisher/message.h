#pragma once

#include "nlohmann/json.hpp"

#include "price4.h"
#include "types.h"

namespace notify
{

struct mTrade
{
    lib::t_price price_;
    lib::t_quantity quantity_;
    std::string type_ = "TRADE";
    
    mTrade(lib::t_price price, lib::t_quantity quantity) : price_(price), quantity_(quantity) {};
    nlohmann::json to_json();
};


struct mUpdateDepth
{
    lib::t_price price_;
    lib::t_quantity quantity_;
    std::string action_; // {ADD, DELETE, MODIFY}
    
    mUpdateDepth(lib::t_price price, lib::t_quantity quantity, std::string action) : price_(price), quantity_(quantity), action_(action) {};
    nlohmann::json to_json();
};

inline nlohmann::json mTrade::to_json()
{
    return nlohmann::json{{"type", "TRADE"},
        {"price", std::to_string(1.0 * price_/10000)},
        {"quantity", quantity_}};
};

inline nlohmann::json mUpdateDepth::to_json()
{
    return nlohmann::json{{"type", action_},
        {"price", std::to_string(1.0 * price_/10000)},
        {"quantity", quantity_}};
};


} // namespace notify

