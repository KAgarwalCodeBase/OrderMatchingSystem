#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include "Order.h"
#include "AVLTree.h"
#include "Logger.h" // Include the Logger
#include <vector>
#include <utility>

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

        Logger::getInstance().log("Order added to book: ID = " + std::to_string(order.orderId) +
                                  ", Side = " + (order.side == 'B' ? "Buy" : "Sell") +
                                  ", Price = " + std::to_string(order.price) +
                                  ", Quantity = " + std::to_string(order.quantity));
    }

    // Match an incoming order with existing orders in the book
    std::pair<bool, Order> matchOrder(Order& incomingOrder) {
        Logger& logger = Logger::getInstance();

        if (incomingOrder.side == 'B') {
            // Find the lowest sell order (price <= incoming price)
            auto sellOrdersAtPrice = sellOrders.findLowestLessThanEqual(incomingOrder.price);
            if (!sellOrdersAtPrice.empty()) {
                Order matchedOrder = sellOrdersAtPrice.front(); // First order to match

                logger.log("Matching Buy Order ID: " + std::to_string(incomingOrder.orderId) +
                           " with Sell Order ID: " + std::to_string(matchedOrder.orderId));

                // Match logic
                if (incomingOrder.quantity >= matchedOrder.quantity) {
                    incomingOrder.quantity -= matchedOrder.quantity;
                    sellOrdersAtPrice.erase(sellOrdersAtPrice.begin()); // Remove fully matched order
                } else {
                    matchedOrder.quantity -= incomingOrder.quantity;
                    incomingOrder.quantity = 0;
                }

                // Reinsert unmatched remaining orders
                for (const auto& order : sellOrdersAtPrice) {
                    sellOrders.insert(order.price, order);
                }

                return {true, matchedOrder};
            }
        } else if (incomingOrder.side == 'S') {
            // Find the highest buy order (price >= incoming price)
            auto buyOrdersAtPrice = buyOrders.findHighestGreaterThanEqual(incomingOrder.price);
            if (!buyOrdersAtPrice.empty()) {
                Order matchedOrder = buyOrdersAtPrice.front(); // First order to match

                logger.log("Matching Sell Order ID: " + std::to_string(incomingOrder.orderId) +
                           " with Buy Order ID: " + std::to_string(matchedOrder.orderId));

                // Match logic
                if (incomingOrder.quantity >= matchedOrder.quantity) {
                    incomingOrder.quantity -= matchedOrder.quantity;
                    buyOrdersAtPrice.erase(buyOrdersAtPrice.begin()); // Remove fully matched order
                } else {
                    matchedOrder.quantity -= incomingOrder.quantity;
                    incomingOrder.quantity = 0;
                }

                // Reinsert unmatched remaining orders
                for (const auto& order : buyOrdersAtPrice) {
                    buyOrders.insert(order.price, order);
                }

                return {true, matchedOrder};
            }
        }

        // No match found
        logger.log("No match found for Order ID: " + std::to_string(incomingOrder.orderId));
        return {false, Order()};
    }
};

#endif // ORDER_BOOK_H
