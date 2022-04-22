#include <stdio.h>

#include <fstream>
#include <iostream>

#include "nlohmann/json.hpp"

#include "order.h"
#include "order_generator.h"

using namespace lob;


// Set the initial global order ID
lib::t_orderid OrderGenerator::globalOrderId = 10000000;

// Table 1 from A. Subrahmanyam and H. Zheng, 2016, Limit order placement by high-frequency traders
// About half of the orders are cancel orders.
double OrderGenerator::cancelRatio = 0.4;

lib::t_price OrderGenerator::genPrice(lib::TickSizeRule& tsr)
{
    const std::vector<lib::Tick>& ticks = tsr.GetTicks();
    
    lib::t_price price = static_cast<lib::t_price>(m_distPrice(m_gen));
    
    for(auto tick : ticks)
    {
        if(price * 10000 <= tick.to_price.unscaled())
        {
            double price_adj = static_cast<lib::t_price>(m_distPrice(m_gen)) * tick.tick_size;
            return (price + price_adj) * PowScale4;
        }
    }
    return price * PowScale4;
}

OrderGenerator::OrderGenerator(lib::t_symbol symbol): m_symbol(symbol)
{
    m_fileName = "orders_" + m_symbol + ".json";

    m_gen = std::mt19937(rd());
    m_distReal = std::uniform_real_distribution<double>(0.0, 1.0);
    m_distQuantity = std::uniform_int_distribution<lib::t_quantity>(1, 100);
    m_distPrice = std::lognormal_distribution<double>(5.0, 0.1);
}


lib::t_orderid OrderGenerator::genCancelOrderId()
{
    // The cancel order id should be an uncancelled order id.
    lib::t_orderid size_orderIds = m_orderIds.size();
    m_distOrderId = std::uniform_int_distribution<lib::t_orderid>(0, size_orderIds - 1);
    auto it = m_orderIds.begin();
    std::advance(it, m_distOrderId(m_gen));
    lib::t_orderid res = *it;
    
    // Take out the cancelled id
    m_orderIds.erase(it);

    return res;
}

void OrderGenerator::run(lib::TickSizeRule& tsr, lib::t_lot lot)
{
    std::fstream f(m_fileName, std::ios::out);

    // For simplicity, generate one order per second
    for (int i = 0; i < 1000; i++)
    {
        lib::OrderStatus orderStatus = genOrderStatus();
        nlohmann::json j;
        if (m_orderIds.empty() || orderStatus == lib::OrderStatus::NEW)
        {
            m_orderIds.insert(globalOrderId);
            Order myOrder(duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + i, m_symbol, globalOrderId++, genOrderSide(), genPrice(tsr), genQuantity(lot), orderStatus);
            myOrder.to_json(j);
        }
        else
        {
            Order myOrder(duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() + i, genCancelOrderId());
            myOrder.to_json(j);
        }
        
        f << j << std::endl;
    }
}


