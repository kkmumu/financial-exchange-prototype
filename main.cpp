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

#include "price4.h"
#include "types.h"
#include "ticks.h"
#include "order.h"
#include "book.h"
#include "engine.h"
//#include "publisher.h"

#include "nlohmann/json.hpp"

typedef Order t_order;
using json = nlohmann::json;

int main()
{
    
    MatchingEngine eng;
    std::vector<Order> orders;
    
    eng.config("config.json");
    orders = eng.orderParser("orders.jsonl");
    
    //TimeInForce obj = orders[7].time_in_force();
    
    return 0;
}
