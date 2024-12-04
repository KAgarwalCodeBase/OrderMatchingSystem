#include "NetworkInterface.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <stdexcept>
#include <arpa/inet.h>
#include <unistd.h>
#include <cmath> // For round function
#include "Logger.h"

// Constructor
NetworkInterface::NetworkInterface(int port) {
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        throw std::runtime_error("Failed to create socket.");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        throw std::runtime_error("Failed to bind socket to port.");
    }
}



// Parses an order string into an Order object
Order NetworkInterface::parseOrder(const std::string& orderStr) {
    Logger& logger = Logger::getInstance(); // Use Logger for logging
    std::istringstream ss(orderStr);
    int orderId, quantity, timestamp, traderId;
    char side;
    double price;
    int isMarketOrder;

    ss >> orderId >> side >> price >> quantity >> timestamp >> traderId >> isMarketOrder;

    // Validate side
    if (side != 'B' && side != 'S') {
        logger.log("Validation Error: Invalid side '" + std::string(1, side) + "'. Must be 'B' or 'S'.");
        throw std::invalid_argument("Invalid side: must be 'B' or 'S'.");
    }

    // Validate price
    if (price <= 0.0) {
        logger.log("Validation Error: Price must be greater than zero. Received: " + std::to_string(price));
        throw std::invalid_argument("Price must be greater than zero.");
    }
    
    // No validation for timestamp
        logger.log("Timestamp received: " + std::to_string(timestamp));


    // Validate quantity
    if (quantity <= 0) {
        logger.log("Validation Error: Quantity must be greater than zero. Received: " + std::to_string(quantity));
        throw std::invalid_argument("Quantity must be greater than zero.");
    }

    // Validate isMarketOrder
    if (isMarketOrder != 0 && isMarketOrder != 1) {
        logger.log("Validation Error: isMarketOrder must be 0 or 1. Received: " + std::to_string(isMarketOrder));
        throw std::invalid_argument("isMarketOrder must be 0 or 1.");
    }

    // Log successful parsing
    logger.log("Order parsed successfully: ID=" + std::to_string(orderId) +
               ", Side=" + std::string(1, side) +
               ", Price=" + std::to_string(price) +
               ", Quantity=" + std::to_string(quantity) +
               ", Timestamp=" + std::to_string(timestamp) +
               ", TraderID=" + std::to_string(traderId) +
               ", MarketOrder=" + std::to_string(isMarketOrder));

    return Order(orderId, side, price, quantity, timestamp, traderId, isMarketOrder == 1);
}


// Receives and processes incoming orders
void NetworkInterface::receiveOrders(MatchingEngine& engine) {
    char buffer[1024];
    while (true) {
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        int bytesReceived = recvfrom(socket_fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0'; // Null-terminate the string
            std::string orderStr(buffer);

            // Parse the order
            try {
                auto order = parseOrder(orderStr); // Create a parseOrder function to map strings to Order objects
                engine.processOrder(order);

                // Send success response
                std::string response = "Order processed successfully.";
                sendto(socket_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, addrLen);
            } catch (const std::exception& e) {
                // Send error response
                std::string response = "Error processing order: " + std::string(e.what());
                sendto(socket_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, addrLen);
            }
        }
    }
}


