/// @file main.cpp
/// @brief  In this implementation, the limit order book is represented using a flat linear array (pricePoints), indexed by the numeric price value.
///         Each entry in this array corresponds to a specific price point and holds an instance of struct pricePoint. This data structure maintains a list
///         of outstanding buy/sell orders at the respective price. Each outstanding limit order is represented by an instance of struct orderBookEntry.
/// @author Shangwen Sun
/// @date 03/31/2022


/// @ref What is an efficient data structure to model order book?
///      https://quant.stackexchange.com/questions/3783/what-is-an-efficient-data-structure-to-model-order-book
///      QuantCup's winning implementation
///      https://web.archive.org/web/20140529180725/http://www.quantcup.org/
///      https://web.archive.org/web/20141222151051/https://dl.dropboxusercontent.com/u/3001534/engine.c
///
///      Nasdaq ITCH protocal http://www.nasdaqtrader.com/content/technicalsupport/specifications/dataproducts/NQTVITCHSpecification.pdf
///
///      JSON references https://github.com/nlohmann/json
///
/// efficiency:
///  cancel -> O(1)
///
/// optimization:
/// - inline functions
/// - template function to avoid conditional branching
/// - strcpy v.s. memcpy https://stackoverflow.com/questions/24966693/is-memcpy-usually-faster-than-strcpy
///

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>

#include "price4.h"
#include "types.h"
#include "ticks.h"

#include "order.h"
#include "order_entry.h"
#include "order_base.h"
#include "parser.h"
#include "book.h"
#include "order_generator.h"

#include "engine.h"

#include "message.h"
#include "notifier.h"

#include "nlohmann/json.hpp"

using namespace lib;
using namespace lob;

int main()
{
    /*

    /// Test TickSizeRule -> test passed!
    nlohmann::json j_tick = {
        {{"from_price", "0"}, {"to_price", "1"}, {"tick_size", 0.0001}},
        {{"from_price", "1"}, {"tick_size", 0.01}},
    };
    lib::TickSizeRule tsr;
    tsr.FromJson(j_tick);
     
     
    /// Test OrderGenerator -> test passed!
     lob::OrderGenerator myOrderGenerator("MSFT");
     myOrderGenerator.run(tsr, 100, 100);
     
     
    /// Test OrderParser -> test passed!
    std::vector<lob::Order> orders;
    lob::OrderParser parser;
    orders = parser.load("orders.jsonl", tsr, 100);
        
    
    /// Test  LimitOrderBook -> test passed!
    Order order0(1625787615, "AAPL", 1000134, 1, 1403000, 100, lib::OrderStatus::NEW);
    Order order1(1625787616, "AAPL", 1000135, 0, 1402500, 200, lib::OrderStatus::NEW);
    Order order2(1625787617, "AAPL", 1000135, 1, 0, 0, lib::OrderStatus::CANCEL);

    OrderBook book0("AAPL");
    book0.add(order0);
    book0.add(order1);
    book0.add(order2);
     
     
    /// Test MatchingEngine -> test passed!
    eng::MatchingEngine m_eng("config.json");
    lib::TickSizeRule tsr(m_eng.tick_size_rule());

    m_eng.match_orders("orders_AAPL.json");
     
    /// Test Notifier -> test passed!
    notify::Notifier notifier;
    std::cout << notifier.notify_trade(1314000, 100).dump();
    
     */

    
    return 0;
}
