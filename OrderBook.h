#ifndef MY_ORDERBOOK_H
#define MY_ORDERBOOK_H

#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <deque>
#include <map>
#include <cassert>
#include <algorithm>



using Price = std::uint32_t;
using Quantity = std::int32_t;
using TickerOrder = std::string;
using OrderId = std::int32_t;


enum class Side
{
    BUY,
    SELL,
};

struct Order
{
    Price m_price { };
    Quantity m_quantity { };
    TickerOrder m_ticker { };
    OrderId m_orderId { };
    Side m_type { };
};

struct PriceLevel
{
    std::deque<Order> orders { };
    Quantity m_totalQuantity { };
};


class OrderBook
{
    private:

        std::map<Price, std::deque<Order>> m_bids { };
        std::map<Price, std::deque<Order>> m_asks { };

    public:

        void add(const Order& newOrder);
        void cancel(const Order& cancelOrder);
        void execute(const Order& executeOrder);
        void printBook() const;
};

#endif //MY_ORDERBOOK_H