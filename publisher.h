/// @file publisher.h
/// @brief This is a file to implement market data publisher.
/// @author Shangwen Sun
/// @date 04/02/2022

#pragma once

#include "nlohmann/json.hpp"

class MarketDataPublisher
{
public:
    MarketDataPublisher() = default;
    ~MarketDataPublisher() = default;
    
    void Trade() const;
      
    void DepthUpdate() const;
    
    nlohmann::json to_json() const;

};

