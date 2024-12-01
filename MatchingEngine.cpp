#include "MatchingEngine.h"
#include <iostream>

void MatchingEngine::processOrder(Order order) {
    auto start = std::chrono::high_resolution_clock::now();

    while (order.quantity > 0) {
        auto [matched, matchedOrder] = orderBook.matchOrder(order);
        if (!matched) {
            break;
        }
        std::cout << "Matched Order: " << matchedOrder.orderId << " with " << order.orderId
                  << " for quantity: " << matchedOrder.quantity << "\n";
    }

    if (order.quantity > 0 && !order.isMarketOrder) {
        orderBook.addOrder(order);
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    std::cout << "Order processing latency: " << duration << " microseconds\n";
}
