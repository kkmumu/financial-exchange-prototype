/// @file engine.h
/// @brief This is a file to implement the matching engine.
/// @author Shangwen Sun
/// @date 03/31/2022

#pragma once
#include <thread>
#include <mutex>  // For std::unique_lock

#include "nlohmann/json.hpp"

#include "price4.h"
#include "types.h"

#include "order.h"
#include "book.h"
#include "parser.h"


// - submit orders
// - submit requests to cancel/replace existing orders
// - give notice if the requests is rejected



namespace eng
{

// the matching engine should be a multi-threading engine, one to deal with loading orders, one to deal with matching orders, one to deal with sending feedback to the outside
class MatchingEngine
{
private:
    lib::TickSizeRule tick_size_rule_;
    lib::t_lot lot_size_;
    
    lob::OrderParser parser; // parser tool to deal with json order files
    lob::OrderBook lob;
    
public:
    MatchingEngine() = default;
    MatchingEngine(const lib::FILE& config_file_name);
    ~MatchingEngine() = default;
    
    lib::t_lot lot_size();
    lib::TickSizeRule& tick_size_rule();
    
    /// @brief start the engine at the begining of a trading day
    void start(const lib::FILE& state_file_last_day);
    
    /// @brief match orders from the request file
    void match_orders(const lib::FILE& order_request_file_name);
    void clear();
};

inline lib::t_lot MatchingEngine::lot_size()
{
    return lot_size_;
}

inline lib::TickSizeRule& MatchingEngine::tick_size_rule()
{
    return tick_size_rule_;
}

}

