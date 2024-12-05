#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <cmath>
#include <string>
#include "Order.h"
#include <cstring>
#include <sstream>
#include "Logger.h"
#include <iostream>
#include <unistd.h>
#include <stdexcept>
#include <arpa/inet.h>
#include "MatchingEngine.h"
#include <vector>
#include <atomic>

class NetworkInterface {
private:
    int socket_fd; // Network socket file descriptor
    int port;      // Port the socket is bound to
    std::atomic<bool> isRunning; // Flag to control receiveOrders loop
    void validateOrder(int orderId, char side, double price, int quantity, int timestamp, int traderId, int isMarketOrder, Logger& logger);

public:
    explicit NetworkInterface(int port);
    ~NetworkInterface();
    void receiveOrders(MatchingEngine& engine);
    void stop();
    Order parseOrder(const std::string& orderStr);
};

#endif // NETWORK_INTERFACE_H
