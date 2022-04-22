#include <iostream>
#include <fstream>

#include "nlohmann/json.hpp"

#include "types.h"
#include "message.h"
#include "notifier.h"

using namespace notify;

void Notifier::Notify(lib::t_price price, lib::t_quantity quantity)
{
    mTrade tmp(price, quantity);
    mtrades.emplace_back(tmp);
    std::cout << tmp.to_json().dump() << std::endl;
}

template<> // update ask book
void Notifier::Notify<1>(lib::t_price price, lib::t_quantity quantity, std::string action)
{
    mUpdateDepth tmp(price, quantity, action);
    mupdates_ask.emplace_back(tmp);
    std::cout << tmp.to_json().dump() << std::endl;
}

template<> // update bid book
void Notifier::Notify<2>(lib::t_price price, lib::t_quantity quantity, std::string action)
{
    mUpdateDepth tmp(price, quantity, action);
    mupdates_bid.emplace_back(tmp);
    std::cout << tmp.to_json().dump() << std::endl;
}


void Notifier::publish()
{
    // write messages to json file
    std::ofstream file(outfile_);
    for(auto& message: mtrades)
        file << message.to_json();
    
    nlohmann::json j = nlohmann::json{{"type", "DEPTH_UPDATE"},
        {"bid", },
        {"ask", }};
    
    for(auto& message: mupdates_bid)
        j["bid"].push_back(message.to_json());
    
    for(auto& message: mupdates_ask)
        j["ask"].push_back(message.to_json());
        
    file << j;
}

