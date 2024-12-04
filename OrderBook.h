#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "Order.h"
#include "AVLTree.h"
#include <vector>

class OrderBook {
private:
    AVLTree buyOrders;  // AVL tree for buy orders (sorted by descending price)
    AVLTree sellOrders; // AVL tree for sell orders (sorted by ascending price)

public:
    // Add a new order to the book
    void addOrder(const Order& order) {
        if (order.side == 'B') {
            buyOrders.insert(order.price, order);
        } else if (order.side == 'S') {
            sellOrders.insert(order.price, order);
        }
    }

    // Match an incoming order with existing orders in the book
    std::pair<bool, Order> matchOrder(Order& incomingOrder) {
        if (incomingOrder.side == 'B') {
            // Find the lowest sell order
            auto sellOrdersAtPrice = sellOrders.find(incomingOrder.price);
            if (!sellOrdersAtPrice.empty()) {
                // Match with the first order at the price level
                Order matchedOrder = sellOrdersAtPrice.front();
                if (incomingOrder.quantity >= matchedOrder.quantity) {
                    incomingOrder.quantity -= matchedOrder.quantity;
                    sellOrdersAtPrice.erase(sellOrdersAtPrice.begin());
                } else {
                    matchedOrder.quantity -= incomingOrder.quantity;
                    incomingOrder.quantity = 0;
                }
                return {true, matchedOrder};
            }
        } else if (incomingOrder.side == 'S') {
            // Find the highest buy order
            auto buyOrdersAtPrice = buyOrders.find(incomingOrder.price);
            if (!buyOrdersAtPrice.empty()) {
                // Match with the first order at the price level
                Order matchedOrder = buyOrdersAtPrice.front();
                if (incomingOrder.quantity >= matchedOrder.quantity) {
                    incomingOrder.quantity -= matchedOrder.quantity;
                    buyOrdersAtPrice.erase(buyOrdersAtPrice.begin());
                } else {
                    matchedOrder.quantity -= incomingOrder.quantity;
                    incomingOrder.quantity = 0;
                }
                return {true, matchedOrder};
            }
        }
        return {false, Order()}; // No match found
    }
};

#endif // ORDER_BOOK_H
