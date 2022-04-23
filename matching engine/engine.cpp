#include <stdexcept>
#include <thread>

#include "nlohmann/json.hpp"

#include "order_generator.h"
#include "engine.h"

using namespace eng;
 
MatchingEngine::MatchingEngine(const lib::FILE& config_file_name) : lot_size_(100)
{
    // configurate tick size rule
    nlohmann::json j_config;
    std::ifstream config(config_file_name, std::ifstream::in);
    
    if (!config.is_open())
    {
        std::cout << "FAILED TO OPEN " + config_file_name + ".\n";
        throw std::exception();
    }
    
    j_config = nlohmann::json::parse(config);
    tick_size_rule_.FromJson(j_config);
}



void MatchingEngine::start(const lib::FILE& state_file_last_day)
{
    std::vector<lob::Order> GTC_orders_last_day;

    GTC_orders_last_day = parser.load(state_file_last_day, tick_size_rule_, lot_size_);
}



void MatchingEngine::match_orders(const lib::FILE& order_request_file_name)
{
    lob::OrderParser parser;
    std::vector<lob::Order> pending_orders;

    pending_orders = parser.load(order_request_file_name, tick_size_rule_, lot_size_);
    
    for(auto order : pending_orders){
        lob.add(order);
    }

    // matching -> thread II
    
}

