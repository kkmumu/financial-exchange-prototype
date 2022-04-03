#include "types.h"
#include "book.h"
#include "engine.h"

// initialize an array of price points to represent a limit order book
void OrderBook::init()
{
    pricePoints.resize(MAX_PRICE);
    for (auto& pp : pricePoints)
    {
        pp.clear();
    }

    arenaBookEntries.resize(MAX_NUM_ORDERS);

    curOrderID = 0;
    askMin = MAX_PRICE;
    bidMax = MIN_PRICE;
}

template <>
void OrderBook::execution_notifier<Side::SELL>(t_orderid bid_id, t_orderid ask_id, t_price match_price, t_quantity match_quantity)
{
    // skip orders that have been cancelled
    if (match_quantity == 0)
        return;
    // exhuast corresponding book entry
    arenaBookEntries[bid_id].quantity -= match_quantity;
    // TO DO: PUBLISH TRADE INFORMATION
}

template <>
void OrderBook::execution_notifier<Side::BUY>(t_orderid bid_id, t_orderid ask_id, t_price match_price, t_quantity match_quantity)
{
    // skip orders that have been cancelled
    if (match_quantity == 0)
        return;
    // exhuast corresponding book entry
    arenaBookEntries[ask_id].quantity -= match_quantity;
    // TO DO: PUBLISH TRADE INFORMATION
}

// dealing with simple limit day order first
template <>
t_orderid OrderBook::addOrder<Side::BUY, OrderStatus::NEW, OrderType::LIMIT, TimeInForce::DAY>(t_order& order)
{
    t_price orderPrice = order.price();
    t_quantity orderSize = order.quantity();
    t_orderid orderID = order.orderid();
    
    // look for outstanding ask orders that cross with the incoming order
    if(orderPrice >= askMin)
    {
        // if there is matching price in the ask array
        auto ppEntry = pricePoints.begin() + askMin;
        
        do
        {
            auto bookEntry = ppEntry->begin();
            while (bookEntry != ppEntry->end())
            {
                if (bookEntry->quantity < orderSize)
                {
                    execution_notifier<Side::BUY>(order.orderid(), bookEntry->order_id, orderPrice, bookEntry->quantity);
                    orderSize -= bookEntry->quantity;
                    ++bookEntry; // move on to next bookEntry at the same price level
                }
                else
                {
                    execution_notifier<Side::BUY>(order.orderid(), bookEntry->order_id, orderPrice, bookEntry->quantity);
                    // if current bookEntry cannot be fully exhuasted // move on to next bookEntry at the same price level
                    if (bookEntry->quantity > orderSize)
                        bookEntry->quantity -= orderSize;
                    else
                        ++bookEntry;
                    // otherwise current bookEntry is fully exhausted and can be removed from current book
                    ppEntry->erase(ppEntry->begin(), bookEntry);
                    // while book entries at current price level are not exhuasted
                    while (ppEntry->begin() != bookEntry)
                    {
                        ppEntry->pop_front();
                    }
                    return ++curOrderID;
                }
            }
            // We have exhausted all orders at the askMin price point. Move on to next price level
            ppEntry->clear();
            ppEntry++;
            askMin++; // update askMin
        }while(orderPrice >= askMin);
        
    }
    else
    {
        // if no matching price, add the order to current bid array
        auto entry = arenaBookEntries.begin() + (++curOrderID);
        entry->quantity = orderSize;
        entry->order_id = orderID;
        pricePoints[orderPrice].push_back(*entry);
        
        // update bidMax if order price is larger than it
        if (bidMax < orderPrice) bidMax = orderPrice;
    }

    return curOrderID;
}



template <>
t_orderid OrderBook::addOrder<Side::SELL, OrderStatus::NEW, OrderType::LIMIT, TimeInForce::DAY>(t_order& order)
{
    t_price orderPrice = order.price();
    t_quantity orderSize = order.quantity();
    t_orderid orderID = order.orderid();

    
    // look for outstanding bid orders that cross with the incoming order
    if(orderPrice <= bidMax)
    {
        // if there is matching price in the ask array
        auto ppEntry = pricePoints.begin() + bidMax;
        do
        {
            auto bookEntry = ppEntry->begin();
            while (bookEntry != ppEntry->end())
            {
                if (bookEntry->quantity < orderSize)
                {
                    execution_notifier<Side::SELL>(bookEntry->order_id, order.orderid(), orderPrice, bookEntry->quantity);
                    orderSize -= bookEntry->quantity;
                    ++bookEntry; // move on to next bookEntry at the same price level
                }
                else
                {
                    execution_notifier<Side::SELL>(bookEntry->order_id, order.orderid(), orderPrice, bookEntry->quantity);
                    // if current bookEntry cannot be fully exhuasted // move on to next bookEntry at the same price level
                    if (bookEntry->quantity > orderSize)
                        bookEntry->quantity -= orderSize;
                    else
                        ++bookEntry;
                    // otherwise current bookEntry is fully exhausted and can be removed from current book
                    ppEntry->erase(ppEntry->begin(), bookEntry);
                    // while book entries at current price level are not exhuasted
                    while (ppEntry->begin() != bookEntry)
                    {
                        ppEntry->pop_front();
                    }
                    return ++curOrderID;
                }
            }
            // We have exhausted all orders at the bidMax price point. Move on to next price level
            ppEntry->clear();
            ppEntry--;
            bidMax--; // update bidMax
        }while(orderPrice <= bidMax);
        
    }
    else
    {
        // if no matching price, add the order to current ask array
        auto entry = arenaBookEntries.begin() + (++curOrderID);
        entry->quantity = orderSize;
        entry->order_id = orderID;
        pricePoints[orderPrice].push_back(*entry);
        
        // update askMin if order price is smaller than it
        if (askMin > orderPrice) askMin = orderPrice;
    }

    return curOrderID;
}
 
//template <>
//t_orderid OrderBook::addOrder<Side::SELL, OrderStatus::NEW, OrderType::MARKET, >(t_order& order)
//{
//
//}
