#include <type_traits>

#include "types.h"
#include "ticks.h"
#include "book.h"
//#include "parser.h"
//#include "notifier.h"
//#include "engine.h"

using namespace lob;

OrderBook::OrderBook(const lib::t_symbol& symbol) : symbol_(symbol)
{

    pricePoints.resize(MAX_PRICEPOINT_NUM);

    for (auto& pp : pricePoints)
        pp.clear();

    arenaBookEntries.resize(MAX_NUM_ORDERS);

    curOrderID = 0;
    
    askMin = MAX_PRICE;
    bidMax = MIN_PRICE;

}

void OrderBook::set_symbol(const lib::t_symbol& symbol)
{
    symbol_ = symbol;
}

const lib::t_symbol& OrderBook::symbol() const
{
    return symbol_;
}

lib::t_price OrderBook::best_ask() const
{
    return askMin;
}

lib::t_price OrderBook::best_bid() const
{
    return bidMax;
}

void OrderBook::cancel(lib::t_orderid request_id)
{
    bool found = false;

    // NOTIFY: UPDATE CURRENT BOOK
    try
    {
        arenaBookEntries[request_id].open_qty = 0;
        arenaBookEntries[request_id].order_qty = 0;
        found = true;
    }
    catch(const std::exception& err)
    {
        std::cout << "Invalid CANCEL order ID!" << std::endl;
    }
}

bool OrderBook::add(const Order& order)
{
    bool matched = false;
    
    // CANCEL ORDER
    if(order.status() == lib::OrderStatus::CANCEL)
        cancel(order.orderid());
    
    // NEW ORDER
    lib::t_price orderPrice = order.price();

    OrderBookEntry inbound;
    inbound.order_qty = order.order_qty();
    inbound.open_qty = order.open_qty();
    inbound.order_id = order.orderid();
    
     // LIMIT ORDER, MARKET ORDER, ICEBERG ORDER
    if(order.is_buy())
    {
        matched = match_bid_order(inbound, orderPrice);
        // IOC ORDER
        if(inbound.order_qty && order.immediate_or_cancel())
            cancel(inbound.order_id);
    }
    else
    {
        matched = match_ask_order(inbound, orderPrice);
        // IOC ORDER
        if(inbound.order_qty && order.immediate_or_cancel())
            cancel(inbound.order_id);
    }

    return matched;
}


bool OrderBook::create_trade(OrderBookEntry& inbound, OrderBookEntry& current, lib::t_quantity matched_quantity)
{
    if(current.open_qty == 0)
        return false;
    
    inbound.open_qty -= matched_quantity;
    current.open_qty -= matched_quantity;
    return true;
}

bool OrderBook::insert_order(OrderBookEntry& inbound, lib::t_price orderPrice)
{
    if(orderPrice <= 0)
        return false;
    
    arenaBookEntries[++curOrderID] = std::move(inbound);
    
    auto entry = arenaBookEntries.begin() + (++curOrderID);
    entry->open_qty = inbound.open_qty;
    if(inbound.is_iceberg)
    {
        entry->order_qty = inbound.order_qty;
        entry->is_iceberg = inbound.is_iceberg;
    }
    entry->is_gtc = inbound.is_gtc;
    entry->order_id = inbound.order_id;
    pricePoints[orderPrice].push_back(*entry);
    
    return true;
}

// Try to match order.  Generate trades.
// If not completely filled and not IOC,
// add the order to the order book
bool OrderBook::match_bid_order(OrderBookEntry& entry, lib::t_price orderPrice)
{
    bool matched = false;
    
    // look for outstanding ask orders that cross with the incoming order
    if(orderPrice >= askMin)
    {
        // if there is matching price in the ask array
        auto ppEntry = pricePoints.begin() + askMin;
        
        do
        {
            auto bookEntry = ppEntry->begin();
            // while the ask book is not empty, exhaust existing ask order
            while (bookEntry != ppEntry->end())
            {
                if (bookEntry->open_qty < entry.open_qty)
                {
                    // create trade
                    matched = create_trade(entry, *bookEntry, bookEntry->open_qty);
                    // NOTIFY: UPDATE ASK BOOK
                    std::cout << "DELETE" << std::endl;
                    ++bookEntry; // move on to next bookEntry at the same price level
                }
                else
                {
                    if (bookEntry->open_qty > entry.open_qty)
                    {
                        matched = create_trade(entry, *bookEntry, entry.open_qty);
                        // NOTIFY: UPDATE ASK BOOK
                        std::cout << "MODIFY" << std::endl;
                    }
                    else
                        ++bookEntry; // bookEntry->quantity == orderSize
                    
                    // delete all the book entries before current bookEntry at this price level
                    while (ppEntry->begin() != bookEntry)
                    {
                        ppEntry->pop_front();
                        std::cout << "DELETE" << std::endl;
                    }
                    ++curOrderID;
                    return matched;
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
        insert_order(entry, orderPrice);
        // if no matching price, add the order to current bid array
        // NOTIFY: UPDATE BID BOOK
        std::cout << "ADD" << std::endl;

        // update bidMax if order price is larger than it
        if (bidMax < orderPrice) bidMax = orderPrice;
    }
    
    return matched;
}


bool OrderBook::match_ask_order(OrderBookEntry& entry, lib::t_price orderPrice)
{
    bool matched = false;
    
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
                if (bookEntry->open_qty < entry.open_qty)
                {
                    // create trade
                    matched = create_trade(entry, *bookEntry, bookEntry->open_qty);
                    // NOTIFY: UPDATE BID BOOK
                    std::cout << "DELETE" << std::endl;
                    ++bookEntry; // move on to next bookEntry at the same price level
                }
                else
                {
                    // if current bookEntry cannot be fully exhuasted // move on to next bookEntry at the same price level
                    if (bookEntry->open_qty > entry.open_qty)
                    {
                        matched = create_trade(entry, *bookEntry, entry.open_qty);
                        // NOTIFY: UPDATE BID BOOK
                        std::cout << "MODIFY" << std::endl;
                    }
                    else
                        ++bookEntry;
                    
                    ppEntry->erase(ppEntry->begin(), bookEntry);
                    while (ppEntry->begin() != bookEntry)
                    {
                        // NOTIFY: TRADE

                        // NOTIFY: UPDATE BID BOOK

                        ppEntry->pop_front();
                    }
                    ++curOrderID;
                    return matched;
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
        insert_order(entry, orderPrice);

        // NOTIFY: UPDATE ASK BOOK
        std::cout << "ADD" << std::endl;

        // update askMin if order price is smaller than it
        if (askMin > orderPrice) askMin = orderPrice;
    }
    
    return matched;
}

/*

template <class OrderPtr>
OrderBook<OrderPtr>::OrderBook(const std::string & symbol)
: symbol_(symbol),
  handling_callbacks_(false),
  order_listener_(nullptr),
  trade_listener_(nullptr),
  order_book_listener_(nullptr),
  logger_(nullptr),
  marketPrice_(MARKET_ORDER_PRICE)
{
  callbacks_.reserve(16);  // Why 16?  Why not?
  workingCallbacks_.reserve(callbacks_.capacity());
}



// 1 for buy
template <>
void OrderBook::execution<lib::Side, (lib::Side)1>(lib::t_orderid bid_id, lib::t_orderid ask_id, lib::t_price match_price, lib::t_quantity match_quantity)
{
    // skip orders that have been cancelled
    if (match_quantity == 0)
        return;
    // NOTIFY: PUBLISH TRADE INFORMATION
    notifier.Notify(match_price, match_quantity);
}

// 2 for sell
template <>
void OrderBook::execution<lib::Side, (lib::Side)2>(lib::t_orderid bid_id, lib::t_orderid ask_id, lib::t_price match_price, lib::t_quantity match_quantity)
{
    // skip orders that have been cancelled
    if (match_quantity == 0)
        return;
    // NOTIFY: PUBLISH TRADE INFORMATION
    notifier.Notify(match_price, match_quantity);
}

// process limit buy order
template <>
lib::t_orderid OrderBook::addOrder<lib::Side, (lib::Side)1, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)1, lib::TimeInForce, (lib::TimeInForce)1>(lib::t_order& order)
{
    lib::t_price orderPrice = order.price();
    lib::t_quantity orderSize = order.quantity();
    lib::t_orderid orderID = order.orderid();
    
    // look for outstanding ask orders that cross with the incoming order
    if(orderPrice >= askMin)
    {
        // if there is matching price in the ask array
        auto ppEntry = pricePoints.begin() + askMin;
        
        do
        {
            auto bookEntry = ppEntry->begin();
            // while the ask book is not empty, exhaust existing ask order
            while (bookEntry != ppEntry->end())
            {
                if (bookEntry->quantity < orderSize)
                {
                    // NOTIFY: UPDATE ASK BOOK
                    notifier.Notify<1>(orderPrice, bookEntry->quantity, "DELETE");
                    orderSize -= bookEntry->quantity;
                    ++bookEntry; // move on to next bookEntry at the same price level
                }
                else
                {
                    if (bookEntry->quantity > orderSize)
                    {
                        bookEntry->quantity -= orderSize;
                        // NOTIFY: UPDATE ASK BOOK
                        notifier.Notify<1>(orderPrice, bookEntry->quantity, "MODIFY");
                    }
                    else
                        ++bookEntry;
                    
                    while (ppEntry->begin() != bookEntry)
                    {
                        // NOTIFY: TRADE
                        notifier.Notify(ppEntry->begin()->price, ppEntry->begin()->quantity);
                        // NOTIFY: UPDATE ASK BOOK
                        notifier.Notify<1>(ppEntry->begin()->price, ppEntry->begin()->quantity, "DELETE");
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
        // NOTIFY: UPDATE BID BOOK
        notifier.Notify<2>(orderPrice, orderSize, "ADD");
        
        // update bidMax if order price is larger than it
        if (bidMax < orderPrice) bidMax = orderPrice;
    }
    
    notifier.publish();
    notifier.clear();
    return curOrderID;
}


// process limit sell order
template <>
lib::t_orderid OrderBook::addOrder<lib::Side, (lib::Side)2, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)1, lib::TimeInForce, (lib::TimeInForce)1>(lib::t_order& order)
{
    lib::t_price orderPrice = order.price();
    lib::t_quantity orderSize = order.quantity();
    lib::t_orderid orderID = order.orderid();

    
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
                    // NOTIFY: UPDATE BID BOOK
                    notifier.Notify<2>(orderPrice, bookEntry->quantity, "DELETE");
                    orderSize -= bookEntry->quantity;
                    ++bookEntry; // move on to next bookEntry at the same price level
                }
                else
                {
                    // if current bookEntry cannot be fully exhuasted // move on to next bookEntry at the same price level
                    if (bookEntry->quantity > orderSize)
                    {
                        bookEntry->quantity -= orderSize;
                        // NOTIFY: UPDATE BID BOOK
                        notifier.Notify<2>(orderPrice, bookEntry->quantity, "MODIFY");
                    }
                    else
                        ++bookEntry;
                    while (ppEntry->begin() != bookEntry)
                    {
                        // NOTIFY: TRADE
                        notifier.Notify(ppEntry->begin()->price, ppEntry->begin()->quantity);
                        // NOTIFY: UPDATE BID BOOK
                        notifier.Notify<2>(ppEntry->begin()->price, ppEntry->begin()->quantity, "DELETE");
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
        
        // NOTIFY: UPDATE BID BOOK
        notifier.Notify<1>(orderPrice, orderSize, "ADD");

        // update askMin if order price is smaller than it
        if (askMin > orderPrice) askMin = orderPrice;
    }

    return curOrderID;
}

// market buy order
template <>
lib::t_orderid OrderBook::addOrder<lib::Side, (lib::Side)1, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)2, lib::TimeInForce, (lib::TimeInForce)1>(lib::t_order& order)
{
    lib::t_quantity orderSize = order.quantity();
    //lib::t_orderid orderID = order.orderid();
    lib::t_price orderPrice = std::numeric_limits<lib::t_price>::max();

    // if there is matching price in the ask array
    auto ppEntry = pricePoints.begin() + askMin;

    do{
        auto bookEntry = ppEntry->begin();
        while (bookEntry != ppEntry->end())
        {
            if (bookEntry->quantity < orderSize)
            {
                orderSize -= bookEntry->quantity;
                ++bookEntry; // move on to next bookEntry at the same price level
            }
            else
            {
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

    
    return curOrderID;
}

// market sell order
template <>
lib::t_orderid OrderBook::addOrder<lib::Side, (lib::Side)2, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)2, lib::TimeInForce, (lib::TimeInForce)1>(lib::t_order& order)
{
    lib::t_price orderPrice = order.price();
    lib::t_quantity orderSize = order.quantity();
    lib::t_orderid orderID = order.orderid();


    // if there is matching price in the ask array
    auto ppEntry = pricePoints.begin() + bidMax;
    do
    {
        auto bookEntry = ppEntry->begin();
        while (bookEntry != ppEntry->end())
        {
            if (bookEntry->quantity < orderSize)
            {
                orderSize -= bookEntry->quantity;
                ++bookEntry; // move on to next bookEntry at the same price level
            }
            else
            {
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


    return curOrderID;
}


void OrderBook::process(const std::string &file_name)
{
    OrderParser parser;
    
    std::vector<lob::Order> orders = parser.load(file_name, tick_size_rule_, lot_size_);
    
    std::cout << "djakldj";
    for(auto order : orders)
    {
//        lib::Side S = order.side();
//        lib::OrderStatus O = order.status();
//        lib::OrderType T = order.order_type();
//        lib::TimeInForce P = order.time_in_force();
        
//        int S = to_underlying<lib::Side>(order.side());
//        int O = to_underlying<lib::OrderStatus>(order.status());
//        int T = to_underlying<lib::OrderType>(order.order_type());
//        int F = to_underlying<lib::TimeInForce>(order.time_in_force());
        
        switch(order.status())
        {
            case lib::OrderStatus::NEW:
                switch(order.side())
                {
                    case lib::Side::BUY:
                        switch (order.order_type())
                        {
                            case lib::OrderType::LIMIT:
                                addOrder<lib::Side , (lib::Side)1, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)1, lib::TimeInForce, (lib::TimeInForce)1>(order);
                            case lib::OrderType::MARKET:
                                addOrder<lib::Side , (lib::Side)1, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)2, lib::TimeInForce, (lib::TimeInForce)1>(order);
                            default:
                                break;
                        }
                    case lib::Side::SELL:
                        switch (order.order_type())
                        {
                            case lib::OrderType::LIMIT:
                                addOrder<lib::Side , (lib::Side)2, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)1, lib::TimeInForce, (lib::TimeInForce)1>(order);
                            case lib::OrderType::MARKET:
                                addOrder<lib::Side , (lib::Side)2, lib::OrderStatus, (lib::OrderStatus)1, lib::OrderType, (lib::OrderType)2, lib::TimeInForce, (lib::TimeInForce)1>(order);
                            default:
                                break;
                        }
                    default:
                        break;
                }
            case lib::OrderStatus::CANCEL:
                cancel(order.orderid());
            default:
                break;
        }
    }
}
*/
