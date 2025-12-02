#include "OrderBook.h"
#include <algorithm>
#include <stdexcept>


void OrderBook::add(const Order& newOrder)
{
    if(newOrder.m_type == Side::BUY)
    {
        auto buyPriceIt = m_bids.find(newOrder.m_price);

        if(buyPriceIt != m_bids.end())
        {
            buyPriceIt->second.push_back(newOrder);

            auto buyOrderIt = std::prev(buyPriceIt->second.end());

            m_orderIndex[newOrder.m_orderId] = 
            { 
                Side::BUY, 
                buyPriceIt,
                buyOrderIt
            };
        }
        else
        {
            m_bids[newOrder.m_price] = std::deque<Order>{ newOrder };

            auto buyPriceIter = m_bids.find(newOrder.m_price);
            auto buyOrderIter = buyPriceIter->second.begin();

            m_orderIndex[newOrder.m_orderId] = 
            {
                Side::BUY,
                buyPriceIter,
                buyOrderIter
            };
        }
    }
    else if (newOrder.m_type == Side::SELL)
    {
        auto sellPriceIt = m_asks.find(newOrder.m_price);

        if(sellPriceIt != m_asks.end())
        {
            sellPriceIt->second.push_back(newOrder);
            auto sellOrderIt = std::prev(sellPriceIt->second.end());

            m_orderIndex[newOrder.m_orderId] = 
            { 
                Side::SELL, 
                sellPriceIt, 
                sellOrderIt 
            };
        }
        else
        {
            m_asks[newOrder.m_price] = std::deque<Order>{ newOrder };

            auto sellPriceIter = m_asks.find(newOrder.m_price);
            auto sellOrderIter = sellPriceIter->second.begin();

            m_orderIndex[newOrder.m_orderId] = 
            { 
                Side::SELL, 
                sellPriceIter, 
                sellOrderIter 
            };
        }
    }
}

void OrderBook::cancel(const Order& cancelOrder)
{

    auto orderIter = m_orderIndex.find(cancelOrder.m_orderId);

    if(orderIter == m_orderIndex.end())
    {
        std::cout << "ORDER YOU WANT TO CANCEL DOES NOT EXIST";
        return;
    }

    auto& location = orderIter->second;

    if(location.side == Side::BUY)
    {
        auto buyPriceLevelIter = location.priceLevelIt;
        buyPriceLevelIter->second.erase(location.orderIt);

        if(buyPriceLevelIter->second.empty())
        {
            m_bids.erase(buyPriceLevelIter);
        }

    }
    else if(location.side == Side::SELL)
    {
        auto sellPriceLevelIter = location.priceLevelIt;

        sellPriceLevelIter->second.erase(location.orderIt);

        if(sellPriceLevelIter->second.empty())
        {
            m_asks.erase(sellPriceLevelIter);
        }

    }

    m_orderIndex.erase(orderIter);

}

void OrderBook::execute(const Order& executeOrder)
{
    Quantity remaining = executeOrder.m_quantity;

    if (executeOrder.m_type == Side::BUY)
    {
        while (remaining > 0 && !m_asks.empty())
        {
            auto itLevel = m_asks.begin();
            Price bestAsk = itLevel->first;

            if (executeOrder.m_price < bestAsk)
                break;

            auto& dq = itLevel->second;

            Quantity levelQty = 0;
            for (auto& o : dq)
                levelQty += o.m_quantity;

            if (levelQty == 0)
            {
                for (auto& o : dq)
                    m_orderIndex.erase(o.m_orderId);

                m_asks.erase(itLevel);
                continue;
            }

            double ratio = double(remaining) / double(levelQty);

            for (auto itOrder = dq.begin(); itOrder != dq.end(); )
            {
                Quantity allocation = Quantity(ratio * itOrder->m_quantity);
                allocation = std::min(allocation, itOrder->m_quantity);
                allocation = std::min(allocation, remaining);

                if (allocation > 0)
                {
                    itOrder->m_quantity -= allocation;
                    remaining -= allocation;

                    std::cout << "Filled " << allocation
                              << " from order " << itOrder->m_orderId << "\n";
                }

                if (itOrder->m_quantity == 0)
                {
                    m_orderIndex.erase(itOrder->m_orderId);
                    itOrder = dq.erase(itOrder);
                }
                else
                {
                    auto itMap = m_orderIndex.find(itOrder->m_orderId);
                    if (itMap != m_orderIndex.end())
                    {
                        itMap->second.priceLevelIt = itLevel;
                        itMap->second.orderIt = itOrder;
                    }
                    ++itOrder;
                }

                if (remaining == 0)
                    break;
            }

            if (dq.empty())
                m_asks.erase(itLevel);
        }
    }
    else if (executeOrder.m_type == Side::SELL)
    {
        while (remaining > 0 && !m_bids.empty())
        {
            auto itLevel = std::prev(m_bids.end());
            Price bestBid = itLevel->first;

            if (executeOrder.m_price > bestBid)
                break;

            auto& dq = itLevel->second;

            Quantity levelQty = 0;
            for (auto& o : dq)
                levelQty += o.m_quantity;

            if (levelQty == 0)
            {
                for (auto& o : dq)
                    m_orderIndex.erase(o.m_orderId);

                m_bids.erase(itLevel);
                continue;
            }

            double ratio = double(remaining) / double(levelQty);

            for (auto itOrder = dq.begin(); itOrder != dq.end(); )
            {
                Quantity allocation = Quantity(ratio * itOrder->m_quantity);
                allocation = std::min(allocation, itOrder->m_quantity);
                allocation = std::min(allocation, remaining);

                if (allocation > 0)
                {
                    itOrder->m_quantity -= allocation;
                    remaining -= allocation;

                    std::cout << "Filled " << allocation
                              << " from order " << itOrder->m_orderId << "\n";
                }

                if (itOrder->m_quantity == 0)
                {
                    m_orderIndex.erase(itOrder->m_orderId);
                    itOrder = dq.erase(itOrder);
                }
                else
                {
                    auto itMap = m_orderIndex.find(itOrder->m_orderId);
                    if (itMap != m_orderIndex.end())
                    {
                        itMap->second.priceLevelIt = itLevel;
                        itMap->second.orderIt = itOrder;
                    }
                    ++itOrder;
                }

                if (remaining == 0)
                    break;
            }

            if (dq.empty())
                m_bids.erase(itLevel);
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

