#include "OrderBook.h"
#include <algorithm>
#include <iostream>

void OrderBook::addOrder(const Order& order) {
    std::lock_guard<std::mutex> lock(bookMutex);
    if (order.side == 'B') {
        buyOrders[-order.price].push_back(order); // Negative price for descending order
    } else {
        sellOrders[order.price].push_back(order);
    }
}

void OrderBook::removeOrder(int orderId, char side) {
    std::lock_guard<std::mutex> lock(bookMutex);
    auto& orders = (side == 'B') ? buyOrders : sellOrders;
    for (auto it = orders.begin(); it != orders.end(); ++it) {
        auto& orderList = it->second;
        for (auto orderIt = orderList.begin(); orderIt != orderList.end(); ++orderIt) {
            if (orderIt->orderId == orderId) {
                orderList.erase(orderIt);
                if (orderList.empty()) {
                    orders.erase(it);
                }
                return;
            }
        }
    }
}

std::pair<bool, Order> OrderBook::matchOrder(Order& incomingOrder) {
    std::lock_guard<std::mutex> lock(bookMutex);
    auto& oppositeOrders = (incomingOrder.side == 'B') ? sellOrders : buyOrders;
    for (auto it = oppositeOrders.begin(); it != oppositeOrders.end(); ++it) {
        double priceLevel = (incomingOrder.side == 'B') ? it->first : -it->first;
        if ((incomingOrder.side == 'B' && priceLevel <= incomingOrder.price) ||
            (incomingOrder.side == 'S' && priceLevel >= incomingOrder.price) ||
            incomingOrder.isMarketOrder) {
            auto& orderList = it->second;
            if (!orderList.empty()) {
                Order matchedOrder = orderList.front();
                int matchQty = std::min(incomingOrder.quantity, matchedOrder.quantity);

                // Adjust quantities
                incomingOrder.quantity -= matchQty;
                matchedOrder.quantity -= matchQty;

                if (matchedOrder.quantity == 0) {
                    orderList.pop_front();
                    if (orderList.empty()) {
                        oppositeOrders.erase(it);
                    }
                } else {
                    orderList.front() = matchedOrder;
                }

                return {true, matchedOrder};
            }
        }
    }
    return {false, Order(-1, 'X', 0.0, 0, 0, 0, false)};
}
