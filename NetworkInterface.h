#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include "MatchingEngine.h"
#include "Order.h"
#include <string>

class NetworkInterface {
private:
    int socket_fd; // Network socket file descriptor
public:
    explicit NetworkInterface(int port); // Constructor
    void receiveOrders(MatchingEngine& engine); // Receives and processes orders
    Order parseOrder(const std::string& orderStr); // Parses an order string into an Order object
};

#endif // NETWORK_INTERFACE_H
