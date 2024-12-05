#include "NetworkInterface.h"

// Constructor
NetworkInterface::NetworkInterface(int port) : port(port), isRunning(true) {
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        throw std::runtime_error("Failed to create socket.");
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(socket_fd);
        throw std::runtime_error("Failed to bind socket to port.");
    }
}

// Destructor to clean up resources
NetworkInterface::~NetworkInterface() {
    close(socket_fd);
}

// Prepares the socket for communication (used for initialization)
void NetworkInterface::prepareSocket() {
    
    // Check if the socket is valid
    if (socket_fd <= 0)
        throw std::runtime_error("Invalid socket descriptor.");
}

// Graceful shutdown
void NetworkInterface::stop() {
    isRunning = false;
}

// Receives and processes incoming orders
void NetworkInterface::receiveOrders(MatchingEngine& engine) {
    std::vector<char> buffer(1024); // Dynamic buffer for larger input
    Logger& logger = Logger::getInstance();

    while (isRunning) {
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        int bytesReceived = recvfrom(socket_fd, buffer.data(), buffer.size(), 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (bytesReceived < 0) {
            if (isRunning) {
                logger.log("Error: Failed to receive data.");
            }
            continue; // Skip processing on error
        }

        buffer[bytesReceived] = '\0'; // Null-terminate the string
        std::string orderStr(buffer.data());

        try {
            auto order = parseOrder(orderStr);
            engine.processOrder(order);

            std::string response = "Order processed successfully.";
            sendto(socket_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, addrLen);
        } catch (const std::exception& e) {
            std::string response = "Error processing order: " + std::string(e.what());
            sendto(socket_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, addrLen);
        }
    }
}

// Parses an order string into an Order object
Order NetworkInterface::parseOrder(const std::string& orderStr) {
    Logger& logger = Logger::getInstance();
    std::istringstream ss(orderStr);

    int orderId, quantity, timestamp, traderId;
    char side;
    double price;
    int isMarketOrder;

    if (!(ss >> orderId >> side >> price >> quantity >> timestamp >> traderId >> isMarketOrder)) {
        logger.log("Parsing Error: Malformed order string: " + orderStr);
        throw std::invalid_argument("Malformed order string");
    }

    // Validate order
    validateOrder(orderId, side, price, quantity, timestamp, traderId, isMarketOrder, logger);

    logger.log("Order parsed successfully: ID=" + std::to_string(orderId) +
               ", Side=" + std::string(1, side) +
               ", Price=" + std::to_string(price) +
               ", Quantity=" + std::to_string(quantity) +
               ", Timestamp=" + std::to_string(timestamp) +
               ", TraderID=" + std::to_string(traderId) +
               ", MarketOrder=" + std::to_string(isMarketOrder));

    return Order(orderId, side, price, quantity, timestamp, traderId, isMarketOrder == 1);
}

// Validates order fields
void NetworkInterface::validateOrder(int orderId, char side, double price, int quantity, int timestamp, int traderId, int isMarketOrder, Logger& logger) {
    std::vector<std::string> errors;

    (void)orderId;  // Suppress unused parameter warning
    (void)traderId; // Suppress unused parameter warning

    // Validate side
    if (side != 'B' && side != 'S') {
        errors.push_back("Invalid side: must be 'B' or 'S'. Received: '" + std::string(1, side) + "'.");
    }

    // Validate price
    if (price <= 0.0) {
        errors.push_back("Price must be greater than zero. Received: " + std::to_string(price));
    }

    // Validate quantity
    if (quantity <= 0) {
        errors.push_back("Quantity must be greater than zero. Received: " + std::to_string(quantity));
    }

    // Validate isMarketOrder
    if (isMarketOrder != 0 && isMarketOrder != 1) {
        errors.push_back("isMarketOrder must be 0 or 1. Received: " + std::to_string(isMarketOrder));
    }

    // Add timestamp logging for debugging (not an error here)
    logger.log("Timestamp received: " + std::to_string(timestamp));

    // If there are errors, throw a single exception
    if (!errors.empty()) {
        std::string errorMsg = "Validation errors: ";
        for (const auto& error : errors) {
            errorMsg += "\n- " + error;
        }
        logger.log(errorMsg); // Log all errors
        throw std::invalid_argument(errorMsg);
    }
}
