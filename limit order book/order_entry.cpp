//
//  order_entry.cpp
//  financial_exchange_prototype
//
//  Created by Sun Shangwen on 4/22/22.
//

#include "order_entry.h"

using namespace lob;

void OrderBookEntry::to_json(nlohmann::json& j)
{
    /*
    if(status_ == lib::OrderStatus::NEW)
    {
        j = nlohmann::json{{"time", timestamp_},
                            {"type", lib::statusStr[status_]},
                            {"order_id", order_id_},
                            {"symbol", symbol_},
                            {"side", lib::sideStr[is_buy_]},
                            {"quantity", order_qty_},
                            {"limit_price", std::to_string(1.0 * price_/10000)}};
    }
    else
    {
        j = nlohmann::json{{"time", timestamp_},
                            {"type", lib::statusStr[status_]},
                            {"order_id", order_id_}};
    }
    */
}
