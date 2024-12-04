#include "MatchingEngine.h"
#include "Logger.h"
#include <ctime>

void MatchingEngine::processOrder(Order order) {
    Logger& logger = Logger::getInstance();
    auto start = std::chrono::high_resolution_clock::now();

    bool matched = false;
    std::string matchedWith = "None";

    // Try to match the order
    while (order.quantity > 0) {
        auto [isMatched, matchedOrder] = orderBook.matchOrder(order);
        if (!isMatched) {
            break;
        }

        matched = true;
        matchedWith = "Order ID " + std::to_string(matchedOrder.orderId);
        logger.log("Matched Order: " + std::to_string(order.orderId) +
                   " with " + matchedWith + " for quantity: " + std::to_string(matchedOrder.quantity));
    }

    // If the order is not fully matched, add it to the book
    if (order.quantity > 0) {
        orderBook.addOrder(order);
        logger.log("Order added to book: ID = " + std::to_string(order.orderId) +
                   ", Side = " + (order.side == 'B' ? 'B' : 'S') +
                   ", Price = " + std::to_string(order.price) +
                   ", Quantity = " + std::to_string(order.quantity));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    // Log the result
    logger.logOrder(order.orderId, (order.side == 'B' ? 'B' : 'S'), order.price, order.quantity,
                    (matched ? "Matched" : "Added to Book"), matchedWith, latency);
}

