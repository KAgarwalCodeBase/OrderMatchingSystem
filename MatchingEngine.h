#ifndef MATCHINGENGINE_H
#define MATCHINGENGINE_H

#include "OrderBook.h"
#include "Order.h"
#include <chrono>

class MatchingEngine {
private:
    OrderBook orderBook;

public:
    void processOrder(Order order);
};

#endif // MATCHINGENGINE_H
