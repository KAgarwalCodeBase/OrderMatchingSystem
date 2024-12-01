#ifndef ORDERBOOK_H
#define ORDERBOOK_H

#include "Order.h"
#include <map>
#include <list>
#include <mutex>

class OrderBook {
private:
    std::map<double, std::list<Order>> buyOrders;  // Buy orders (sorted by price descending)
    std::map<double, std::list<Order>> sellOrders; // Sell orders (sorted by price ascending)
    std::mutex bookMutex;

public:
    void addOrder(const Order& order);
    void removeOrder(int orderId, char side);
    std::pair<bool, Order> matchOrder(Order& incomingOrder);
};

#endif // ORDERBOOK_H
