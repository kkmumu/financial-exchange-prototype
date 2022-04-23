/*****************************************************************
 *
 * orderGenerator implementation -> idea from Bao Yi
 *
 *****************************************************************/
#pragma once

#include <chrono>
#include <random>
#include <unordered_set>

#include "types.h"
#include "order.h"


namespace lob
{

// The orderGenerator is used to generate orders but not required by this project.
// It generates the orders and write into one single json file.
class OrderGenerator
{
private:
    static constexpr lib::t_price PowScale4 = 10000LL;

    // A global incrementing order id
    static lib::t_orderid globalOrderId;

    static double cancelRatio;

    std::string m_fileName;
    lib::t_symbol m_symbol;    // Only one symbol for now

    std::random_device rd;
    std::mt19937 m_gen;
    std::uniform_real_distribution<double> m_distReal;
    std::uniform_int_distribution<lib::t_quantity> m_distQuantity;
    std::lognormal_distribution<double> m_distPrice;
    std::uniform_int_distribution<lib::t_orderid> m_distOrderId;
    std::unordered_set<lib::t_orderid> m_orderIds;

    // Randomly generate: orderId (cancel), orderType, orderSide, quantity, and limit price
    lib::t_orderid genCancelOrderId();
    lib::OrderStatus genOrderStatus();
    lib::t_side genOrderSide();
    lib::t_quantity genQuantity(lib::t_lot lot);
    lib::t_price genPrice(lib::TickSizeRule& tsr);

public:
    OrderGenerator() = default;
    OrderGenerator(lib::t_symbol symbol);
    void run(lib::TickSizeRule& tsr, lib::t_lot lot, std::size_t size);
};

inline lib::OrderStatus OrderGenerator::genOrderStatus()
{
    return m_distReal(m_gen) < cancelRatio? lib::OrderStatus::CANCEL : lib::OrderStatus::NEW;
}

inline lib::t_side OrderGenerator::genOrderSide()
{
    // Assume half of the orders are bid orders.
    return m_distReal(m_gen) < 0.5? 1 : 0;
}

inline lib::t_quantity OrderGenerator::genQuantity(lib::t_lot lot)
{
    return m_distQuantity(m_gen) * lot;
}

}

