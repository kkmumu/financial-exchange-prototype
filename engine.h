/// @file engine.h
/// @brief This is a file to implement the matching engine.
/// @author Shangwen Sun
/// @date 03/31/2022

#pragma once

#include "nlohmann/json.hpp"

#include "price4.h"
#include "types.h"
#include "order.h"
#include "book.h"

// the matching engine should be a multi-threading engine, one to deal with loading orders, one to deal with matching orders, one to deal with sending feedback to the outside
class MatchingEngine
{
private:
    TickSizeRule tick_size_rule_;
    t_lot lot_size_ = 100;
    
    OrderBook lob;
    
public:
    
    MatchingEngine() {};
    ~MatchingEngine() = default;
    
    t_lot lot_size()
    {
        return lot_size_;
    }
    
    TickSizeRule tick_size_rule()
    {
        return tick_size_rule_;
    }
    
    std::vector<t_order> orderParser(const std::string &file_name);

    void config(const std::string &config_file);

    void match();
    void clear();
};
