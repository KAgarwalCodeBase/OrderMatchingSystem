#include "MatchingEngine.h"
#include "Logger.h" // Include the Logger class
#include <iostream>
#include <chrono>

void MatchingEngine::processOrder(Order order) {
    Logger& logger = Logger::getInstance();
    auto start = std::chrono::high_resolution_clock::now();

    std::string action;
    std::string matchedWith = "None";

    while (order.quantity > 0) {
        auto [matched, matchedOrder] = orderBook.matchOrder(order);
        if (!matched) {
            action = "Added to Book";
            break;
        }

        // Log each match
        action = "Matched";
        matchedWith = std::to_string(matchedOrder.orderId);
        logger.logOrder(order.orderId, order.side, order.price, order.quantity, action, matchedWith, 0);

        std::cout << "Matched Order: " << matchedOrder.orderId << " with " << order.orderId
                  << " for quantity: " << matchedOrder.quantity << "\n";
    }

    if (order.quantity > 0 && !order.isMarketOrder) {
        orderBook.addOrder(order);
        action = "Added to Book";
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Log final action and processing latency
    logger.logOrder(order.orderId, order.side, order.price, order.quantity, action, matchedWith, duration);

    std::cout << "Order processing latency: " << duration << " microseconds\n";
}
