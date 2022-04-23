/// @file book.h
/// @brief This is a file to implement an limit order book class.
/// @author Shangwen Sun
/// @date 04/01/2022

#pragma once

#include <vector>
#include <limits>
#include <string>
#include <list>

#include "boost/noncopyable.hpp"
#include "boost/intrusive/slist.hpp"
#include "boost/intrusive/list.hpp"

// namespace lib
#include "types.h"

// namespace lob
#include "order.h"
#include "order_entry.h"
//#include "parser.h"

// namespace notify
//#include "notifier.h"

namespace lob
{

#define MAX_NUM_ORDERS 10010000
#define MAX_LIVE_ORDERS 10010000
#define MAX_ORDER_QUANTITY 10001000
#define MAX_PRICEPOINT_NUM 10000000

/// @brief The limit order book of a security.
class OrderBook : public boost::noncopyable
{
    /* we make this class noncopyable because it's a recursive data structure with
     * pointers to numerous smaller copies of itself maintained using list of pointers
     * to those smaller versions. copying won't make sense if we just copy the pointers,
     * and we also don't want to copy because the structure is quite big (usually).
     */
    
public:
    typedef boost::intrusive::slist<OrderBookEntry, boost::intrusive::cache_last<true> > pricePoint; // describes a single price point in the limit order book.
    
public:
    /// @brief construct
    OrderBook(const lib::t_symbol& symbol = "unknown");

    //OrderBook(const std::string& notify_file_path, const nlohmann::json& tick_json, lib::t_lot lot_size);
    ~OrderBook() = default;
    
    /// @brief Set symbol for orders in this book.
    void set_symbol(const lib::t_symbol& symbol);
    
    /// @brief Get the symbol for orders in this book
    const lib::t_symbol& symbol() const;
    
    /// @brief Get current market price on the ask side.
    lib::t_price best_ask() const;
    
    /// @brief Get current market price on the bid side.
    lib::t_price best_bid() const;
    

    /// @brief add an order to book
    /// @param order the order to add
    /// @return true if the add resulted in a fill
    bool add(const Order& order);
    
    
    
    /// @brief cancel an order in the book
    void cancel(lib::t_orderid request_id);
    
    /// @brief replace an order in the book
    /// @param order the order to replace
    /// @param size_delta the change in size for the order (positive or negative)
    /// @param new_price the new order price, or PRICE_UNCHANGED
    /// @return true if the replace resulted in a fill
    bool replace(const Order& order,
                         lib::t_quantity size_delta,
                         lib::t_price new_price);

    
    ///@brief shutdown the orderbook at the end of a trading day
    void shutdown();
    
protected:
    /// @brief match a new bid order to current orders
    /// @return true if a match occurred
    bool match_bid_order(OrderBookEntry& entry, lib::t_price orderPrice);
    
    /// @brief match a new ask order to current orders
    /// @return true if a match occurred
    bool match_ask_order(OrderBookEntry& entry, lib::t_price orderPrice);
    
    /// @brief perform fill on two orders
    bool create_trade(OrderBookEntry& inbound_tracker, OrderBookEntry& current_tracker, lib::t_quantity matched_quantity);

    /// @brief insert a new order into arenaorderbook at a specific price level
    bool insert_order(OrderBookEntry& inbound, lib::t_price orderPrice);
    
 
private:
    
    lib::t_symbol symbol_;

    // An array of pricePoint structures representing the entire limit order book
    std::vector<OrderBookEntry> arenaBookEntries;
    std::vector<pricePoint> pricePoints;

    // Monotonically-increasing orderID -> access order via order_id -> O(1)
    lib::t_orderid curOrderID;
    
    // Minimum Ask price -> O(1)
    lib::t_price askMin;
    
    // Maximum Bid price -> O(1)
    lib::t_price bidMax;
    
    //notify::Notifier notifier;
};


} // namespace lob
