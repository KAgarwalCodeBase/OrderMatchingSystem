#ifndef ORDER_H
#define ORDER_H

#include <string>

struct Order {
    int orderId;          // Unique order ID
    char side;            // 'B' for Buy, 'S' for Sell
    double price;         // Order price
    int quantity;         // Quantity of the order
    int timestamp;        // Timestamp (e.g., microseconds)
    int traderId;         // Trader ID
    bool isMarketOrder;   // True if market order, false if limit order

    Order(int id, char s, double p, int q, int t, int trader, bool market)
        : orderId(id), side(s), price(p), quantity(q), timestamp(t), traderId(trader), isMarketOrder(market) {}
};

#endif // ORDER_H
