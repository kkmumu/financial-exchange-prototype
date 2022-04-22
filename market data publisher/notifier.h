/// @file publisher.h
/// @brief This is a file to implement market data publisher.
/// @author Shangwen Sun
/// @date 04/02/2022
///
///
/// Notifications intended for trader submitting an order:
// - Order accepted
// - Order rejected
// - Order filled (full or partial)
// - Order replaced
// - Replace request rejected
// - Order canceled
// - Cancel request rejected.
/// Notifications intended to be published as Market Data
// - Trade
// Note this should also trigger the application to do what it needs to do to make the trade happen.
// - Security changed
// Triggered by any event that effects a security
// - Does not include rejected requests.
// Notification of changes in the depth book (if enabled)
// - Depth book changed
// Best Bid or Best Offer (BBO) changed.

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

    std::vector<mTrade> mtrades;
    std::vector<mUpdateDepth> mupdates_bid;
    std::vector<mUpdateDepth> mupdates_ask;

    
public:
    Notifier() = default;
    Notifier(std::string file_name) : outfile_(file_name)
    {
        mtrades.reserve(MAX_MESSAGE_NUM);
        mupdates_bid.reserve(MAX_MESSAGE_NUM);
        mupdates_ask.reserve(MAX_MESSAGE_NUM);
    };
    
    ~Notifier() = default;
    
    void Notify(lib::t_price price, lib::t_quantity quantity);
    
    template<int side> // side = 1 -> bid, side = 2 -> ask
    void Notify(lib::t_price price, lib::t_quantity quantity, std::string act);
    
    void publish();
    void clear();
};


inline void Notifier::clear()
{
    mtrades.clear();
    mupdates_bid.clear();
    mupdates_ask.clear();
}

} // namespace notify
