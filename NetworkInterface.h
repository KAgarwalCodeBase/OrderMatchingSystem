#ifndef NETWORK_INTERFACE_H
#define NETWORK_INTERFACE_H

#include <string>
#include <atomic>
#include <vector>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>
#include "Order.h"
#include "Logger.h"
#include "MatchingEngine.h"

// Manages network communication for receiving and processing orders
class NetworkInterface {
private:
    int socket_fd;                  // Network socket file descriptor
    int port;                       // Port the socket is bound to
    std::atomic<bool> isRunning;    // Flag to control receiveOrders loop

    // Validates order fields to ensure correctness
    void validateOrder(int orderId, char side, double price, int quantity, int timestamp, int traderId, int isMarketOrder, Logger& logger);

public:
    explicit NetworkInterface(int port); // Constructor to initialize with a port
    ~NetworkInterface();                 // Destructor to clean up resources

    void prepareSocket();                      // Prepares the socket for communication
    void receiveOrders(MatchingEngine& engine); // Receives and processes incoming orders
    void stop();                               // Gracefully stops the network interface
    Order parseOrder(const std::string& orderStr); // Parses an order string into an Order object
};

#endif // NETWORK_INTERFACE_H
