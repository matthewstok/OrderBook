#include "OrderBook.h"
#include <algorithm>
#include <stdexcept>


void OrderBook::add(const Order& newOrder)
{
    if(newOrder.m_type == Side::BUY)
    {
        auto it = m_bids.find(newOrder.m_price);

        if(it != m_bids.end())
        {
            it->second.push_back(newOrder);
        }
        else
        {
            std::deque<Order> d;
            d.push_back(newOrder);
            m_bids[newOrder.m_price] = d;
            
        }
    }
    else if (newOrder.m_type == Side::SELL)
    {
        auto it = m_asks.find(newOrder.m_price);

        if(it != m_asks.end())
        {
            it->second.push_back(newOrder);
        }
        else
        {
            std::deque<Order> d;
            d.push_back(newOrder);
            m_asks[newOrder.m_price] = d;
        }
    }
}

//later to make faster you can just pass in OrderID and erase the order from there, can also use the PriceLevel possibly later
void OrderBook::cancel(const Order& cancelOrder)
{
    if(cancelOrder.m_type == Side::BUY)
    {
        auto it = m_bids.find(cancelOrder.m_price);

        if(it != m_bids.end())
        {
            auto& dq = it->second;
            std::deque<Order>::iterator iter;

            for(iter = dq.begin(); iter != dq.end(); ++iter)
            {
                if(iter->m_orderId == cancelOrder.m_orderId)
                {   
                    dq.erase(iter);
                    return;
                }
            }
        }
    }
    else if (cancelOrder.m_type == Side::SELL)
    {
        auto it = m_asks.find(cancelOrder.m_price);

        if(it != m_asks.end())
        {
            auto& dq = it->second;

            std::deque<Order>::iterator iter;
            for(iter = dq.begin(); iter != dq.end(); ++iter)
            {
                if(iter->m_orderId == cancelOrder.m_orderId)
                {
                    dq.erase(iter);
                    return;
                }
            }
        }
    }
}

void OrderBook::execute(const Order& executeOrder)
{
    if(executeOrder.m_type == Side::BUY)
    {
        if(m_asks.empty() || executeOrder.m_price < m_asks.begin()->first)
            throw std::runtime_error("NO MATCHING ASKS FOR THIS BUY ORDER");

        Quantity remainingQuantity = executeOrder.m_quantity;

        while(remainingQuantity > 0 && !m_asks.empty())
        {
            auto it_map = m_asks.begin();
            auto& dq = it_map->second;

            Quantity orderQuantity = 0;
            for(auto& o : dq) orderQuantity += o.m_quantity;

            if(orderQuantity == 0) 
                break; 

            float fillRatio = static_cast<float>(remainingQuantity) / static_cast<float>(orderQuantity);

            for(auto& o : dq)
            {
                Quantity allocation = std::max(static_cast<Quantity>(fillRatio * o.m_quantity), Quantity(1));
                allocation = std::min({ allocation, o.m_quantity, remainingQuantity });

                o.m_quantity -= allocation;
                remainingQuantity -= allocation;

                std::cout << "Filled " << allocation << " from order " << o.m_orderId << "\n";

                if(remainingQuantity == 0) break;
            }

            auto it = dq.begin();
            while(it != dq.end())
            {
                if(it->m_quantity == 0)
                    it = dq.erase(it);
                else
                    ++it;
            }
        }
    }
    else if(executeOrder.m_type == Side::SELL)
    {
        if(m_bids.empty() || executeOrder.m_price > m_bids.rbegin()->first)
            throw std::runtime_error("NO MATCHING BUY FOR THIS ASK ORDER");

        Quantity remainingQuantity = executeOrder.m_quantity;

        while(remainingQuantity > 0 && !m_bids.empty())
        {
            auto it_map = --m_bids.end();

            auto& dq = it_map->second;

            Quantity orderQuantity = 0;
            for(auto& o : dq) orderQuantity += o.m_quantity;

            if(orderQuantity == 0) 
                break;

            float fillRatio = static_cast<float>(remainingQuantity) / static_cast<float>(orderQuantity);

            for(auto& o : dq)
            {
                Quantity allocation = std::max(static_cast<Quantity>(fillRatio * o.m_quantity), Quantity(1));
                allocation = std::min({ allocation, o.m_quantity, remainingQuantity });

                o.m_quantity -= allocation;
                remainingQuantity -= allocation;

                std::cout << "Filled " << allocation << " from order " << o.m_orderId << "\n";

                if(remainingQuantity == 0) break;
            }

            auto it = dq.begin();
            while(it != dq.end())
            {
                if(it->m_quantity == 0)
                    it = dq.erase(it);
                else
                    ++it;
            }
        }
    }
}

void OrderBook::printBook() const
{
    std::cout << "======================== ORDER BOOK ========================\n\n";

    std::cout << "------------------------ BIDS -----------------------------\n";
    if(m_bids.empty())
    {
        std::cout << "No buy orders\n";
    }
    else
    {
        for(auto it = m_bids.rbegin(); it != m_bids.rend(); ++it)
        {
            std::cout << "Price: " << it->first << " | Orders: ";
            for(const auto& order : it->second)
            {
                std::cout << "[ID: " << order.m_orderId
                          << " | Ticker: " << order.m_ticker
                          << " | Qty: " << order.m_quantity << "] ";
            }
            std::cout << '\n';
        }
    }

    std::cout << "\n------------------------ ASKS -----------------------------\n";
    if(m_asks.empty())
    {
        std::cout << "No sell orders\n";
    }
    else
    {
        for(auto it = m_asks.begin(); it != m_asks.end(); ++it)
        {
            std::cout << "Price: " << it->first << " | Orders: ";
            for(const auto& order : it->second)
            {
                std::cout << "[ID: " << order.m_orderId
                          << " | Ticker: " << order.m_ticker
                          << " | Qty: " << order.m_quantity << "] ";
            }
            std::cout << '\n';
        }
    }

    std::cout << "\n===========================================================\n\n";
}

