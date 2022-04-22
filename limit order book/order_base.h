/// @file orderBase.h
/// @brief This is a file to define an order interface.
/// @author Shangwen Sun
/// @date 04/20/2022

#pragma once

namespace lob
{

/// @brief interface an order must implement in order to be used by OrderBook.
class OrderBase
{
public:
    /// @brief is this a limit order?
    bool is_limit() const;

    /// @brief is this order a buy?
    virtual bool is_buy() const = 0;

    /// @brief get the price of this order, or 0 if a market order
    virtual lib::t_price price() const = 0;

    /// @brief get the quantity of this order
    virtual lib::t_quantity order_qty() const = 0;

    /// @brief After generating as many trades as possible against
    /// orders already on the market, cancel any remaining quantity.
    virtual bool immediate_or_cancel() const;

    /// @brief if haven't been traded at the end of a trading day, the outstanding portion of a GTC order persists untill next trading day
    /// and will be inserted into the order book based on the ordering that they appear in the previous day’s order book.
    virtual bool good_till_cancel() const;
};

inline bool OrderBase::is_limit() const
{
  return (price() > 0);
}

inline bool OrderBase::immediate_or_cancel() const
{
  // default to normal
  return false;
}

inline bool OrderBase::good_till_cancel() const
{
  // default to normal
  return false;
}

}
