#pragma once

#include "types.h"
#include "order.h"

#include "boost/intrusive/slist.hpp"
#include "boost/intrusive/list.hpp"


namespace lob
{


struct OrderBookEntry : public boost::intrusive::slist_base_hook<>
{
    lib::t_quantity order_qty{0}; // total order quantity
    lib::t_quantity open_qty{0}; // visible order quantity
    lib::t_orderid order_id;
    bool is_gtc = false;
    bool is_iceberg = false;
    
    void to_json(nlohmann::json& j);
};

}

/*
/// @brief Tracker of an order's state, to keep inside the OrderBook.
///   Kept separate from the order itself.
class OrderEntry
{
public:
    /// @brief construct
    OrderEntry(const Order& order,
               lib::TimeInForce conditions = lib::TimeInForce::UNKNOWN) :
                order_(order),
                open_qty_(order.open_qty()),
                reserved_(0),
                conditions_(conditions) {}

    /// @brief modify the order quantity
    void set_qty(lib::t_quantity new_qty);

    /// @brief fill an order
    void fill(lib::t_quantity qty);

    /// @brief is there no remaining open quantity in this order?
    bool filled() const;

    /// @brief get the total filled quantity of this order
    lib::t_quantity filled_qty() const;

    /// @brief get the open quantity of this order
    lib::t_quantity open_qty() const;

    /// @brief get the order pointer
    const Order& ptr() const;

    /// @brief get the order pointer
    Order& ptr();

    /// @ brief is this order marked good till cancel?
    bool good_till_cancel() const;

    /// @ brief is this order marked immediate or cancel?
    bool immediate_or_cancel() const;

    lib::t_quantity reserve(lib::t_quantity reserved);

public:
    Order order_;
    lib::t_orderid order_id;
    lib::t_quantity open_qty_;
    lib::t_quantity reserved_;
    lib::TimeInForce conditions_;
};

inline lib::t_quantity OrderEntry::reserve(lib::t_quantity reserved)
{
    reserved_ += reserved;
    return open_qty_  - reserved_;
}

inline void OrderEntry::set_qty(lib::t_quantity new_qty)
{
    if ((new_qty < 0 && (int)open_qty_ < std::abs(new_qty)))
        throw std::runtime_error("Replace size reduction larger than open quantity");
    open_qty_ = new_qty;
}

inline void OrderEntry::fill(lib::t_quantity qty)
{
    if (qty > open_qty_)
        throw std::runtime_error("Fill size larger than open quantity");
    open_qty_ -= qty;
}

inline bool OrderEntry::filled() const
{
    return open_qty_ == 0;
}

inline lib::t_quantity OrderEntry::filled_qty() const
{
    return order_.open_qty() - open_qty();
}

// TODO: Rename this to be available and change the rest of the
// system to use that, then provide a method to get to the open
// quantity without considering reserved
inline lib::t_quantity OrderEntry::open_qty() const
{
    return open_qty_ - reserved_;
}

inline const Order& OrderEntry::ptr() const
{
    return order_;
}

inline Order& OrderEntry::ptr()
{
    return order_;
}

inline bool OrderEntry::good_till_cancel() const
{
    return (conditions_ == lib::TimeInForce::GTC);
}

inline bool OrderEntry::immediate_or_cancel() const
{
    return (conditions_ == lib::TimeInForce::IOC);
}

*/
