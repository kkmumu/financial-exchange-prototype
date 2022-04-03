/// @file book.h
/// @brief This is a file to implement an limit order book class.
/// @author Shangwen Sun
/// @date 04/01/2022

#pragma once

#include <vector>
#include <limits>

#include "boost/noncopyable.hpp"
#include "boost/intrusive/slist.hpp"
#include "boost/intrusive/list.hpp"

#include "types.h"
#include "order.h"

#define MAX_NUM_ORDERS 10010000
#define MAX_LIVE_ORDERS 10010000

constexpr t_price MAX_PRICE = std::numeric_limits<t_price>::max();
constexpr t_price MIN_PRICE = 1;

struct OrderBookEntry : public boost::intrusive::slist_base_hook<>
{
    t_price price;
    t_orderid order_id;
    t_time timestamp;
    t_quantity quantity;
    t_quantity quantity_hidden;
    bool visible = true;
};

// pricePoint: describes a single price point in the limit order book.
typedef boost::intrusive::slist<OrderBookEntry, boost::intrusive::cache_last<true> > pricePoint;

class OrderBook : public boost::noncopyable
{
private:
//OrderBook() = default;

    // An array of pricePoint structures representing the entire limit order book
    std::vector<OrderBookEntry> arenaBookEntries;
    std::vector<pricePoint> pricePoints;

    // Monotonically-increasing orderID -> access order via order_id -> O(1)
    t_orderid curOrderID;
    
    // Minimum Ask price -> O(1)
    t_price askMin;
    
    // Maximum Bid price -> O(1)
    t_price bidMax;
    
public:
    OrderBook() {};
    ~OrderBook() = default;
    
    static OrderBook& get()
    {
        static OrderBook lob;
        return lob;
    }

    ///@brief initialize an array of price points to represent a limit order book
    void init();
    
    ///@brief shutdown the orderbook
    void shutdown() {}
    
    ///@brief Process an incoming limit order and return the orderid assigned to order start from 1 and increment with each call
    template <Side S, OrderStatus O, OrderType T, TimeInForce F>
    t_orderid addOrder(t_order& order);
    
    template <Side S>
    void execution_notifier(t_orderid bid_id, t_orderid ask_id, t_price match_price, t_quantity match_quantity);
    
    ///@brief cancel an order by id -> O(1)
    void cancel(t_orderid request_id)
    {
        arenaBookEntries[request_id].quantity = 0;
        arenaBookEntries[request_id].quantity_hidden = 0;
    }
};
