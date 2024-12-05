#include "NetworkInterface.h"

// Constructor: Initializes the socket and binds it to the given port
NetworkInterface::NetworkInterface(int port) : port(port), isRunning(true) {
    // Create a UDP socket
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_fd < 0) {
        throw std::runtime_error("Failed to create socket.");
    }

    // Bind the socket to the specified port
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(socket_fd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        close(socket_fd);
        throw std::runtime_error("Failed to bind socket to port.");
    }
}

// Destructor: Cleans up the socket
NetworkInterface::~NetworkInterface() {
    close(socket_fd);
}

// Prepares the socket for communication (used for initialization)
void NetworkInterface::prepareSocket() {
    if (socket_fd <= 0) {
        throw std::runtime_error("Invalid socket descriptor.");
    }
}

// Gracefully stops the network interface
void NetworkInterface::stop() {
    isRunning = false;
}

// Receives and processes incoming orders from clients
void NetworkInterface::receiveOrders(MatchingEngine& engine) {
    std::vector<char> buffer(1024);  // Buffer to store incoming data
    Logger& logger = Logger::getInstance();

    while (isRunning) {
        sockaddr_in clientAddr;
        socklen_t addrLen = sizeof(clientAddr);

        // Receive data from the client
        int bytesReceived = recvfrom(socket_fd, buffer.data(), buffer.size(), 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (bytesReceived < 0) {
            if (isRunning) {
                logger.log("Error: Failed to receive data.");
            }
            continue;  // Skip processing on error
        }

        buffer[bytesReceived] = '\0';  // Null-terminate the received string
        std::string orderStr(buffer.data());

        // Check for shutdown command
        if (orderStr == "shutdown") {
            logger.log("Shutdown command received. Stopping server...");
            isRunning = false;
            break;
        }

        try {
            // Parse and process the order
            auto order = parseOrder(orderStr);
            engine.processOrder(order);

            // Send a success response back to the client
            std::string response = "Order processed successfully.";
            sendto(socket_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, addrLen);
        } catch (const std::exception& e) {
            // Send an error response to the client
            std::string response = "Error processing order: " + std::string(e.what());
            sendto(socket_fd, response.c_str(), response.length(), 0, (struct sockaddr*)&clientAddr, addrLen);
        }
    }

    logger.log("Server has stopped.");
}

// Parses an order string into an Order object
Order NetworkInterface::parseOrder(const std::string& orderStr) {
    Logger& logger = Logger::getInstance();
    std::istringstream ss(orderStr);

    int orderId, quantity, timestamp, traderId;
    char side;
    double price;
    int isMarketOrder;

    // Parse the order string
    if (!(ss >> orderId >> side >> price >> quantity >> timestamp >> traderId >> isMarketOrder)) {
        logger.log("Parsing Error: Malformed order string: " + orderStr);
        throw std::invalid_argument("Malformed order string");
    }

    // Validate the parsed order
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

// Validates order fields to ensure correctness
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

    // Log timestamp for debugging
    logger.log("Timestamp received: " + std::to_string(timestamp));

    // If there are errors, log and throw an exception
    if (!errors.empty()) {
        std::string errorMsg = "Validation errors: ";
        for (const auto& error : errors) {
            errorMsg += "\n- " + error;
        }
        logger.log(errorMsg); // Log all errors
        throw std::invalid_argument(errorMsg);
    }
}
