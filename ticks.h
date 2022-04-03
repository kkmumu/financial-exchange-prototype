/// @file ticks.h
/// @brief This is a file to implement a class to initialize the tick size rule of the exchange.
/// @author Shangwen Sun
/// @date 04/02/2022

#pragma once
#include "nlohmann/json.hpp"

#include "price4.h"

//Eg. config.json
//[
//  {"from_price": "0", "to_price": "1", "tick_size": "0.0001"},
//  {"from_price": "1", "tick_size": "0.01"},
//]
//[
//    {"lot_size": 100}
//]

struct Tick
{
    Price4 from_price;
    Price4 to_price;
    t_tick tick_size = 0.0;
};

// A class read tick size rule from a JSON file which could support arbitrary breakpoints in the schedule.
class TickSizeRule
{
private:
    std::vector<Tick> ticks;
    
public:
    TickSizeRule() {};
    ~TickSizeRule() = default;
    
    bool FromJson(const nlohmann::json &j);
    
    const std::vector<Tick>& GetTicks() const
    {
        return ticks;
    }
};
