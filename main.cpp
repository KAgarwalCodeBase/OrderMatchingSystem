#include "Logger.h"
#include "Order.h"
#include "NetworkInterface.h"
#include "MatchingEngine.h"
#include <thread>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdexcept>

// Function to initialize critical components
void initializeSystem(MatchingEngine& engine, NetworkInterface& network) {
    Logger::getInstance().log("Initializing critical components...");

    // Pre-warm the matching engine with a dummy order
    Logger::getInstance().log("Pre-warming matching engine...");
    Order dummyOrder(0, 'B', 0.0, 0, 0, 0, false);  // Dummy order
    engine.processOrder(dummyOrder);

    // Pre-warm the network interface
    Logger::getInstance().log("Pre-warming network interface...");
    network.prepareSocket();  // Ensure the socket is ready

    Logger::getInstance().log("Critical components initialized...");
}

int main() {
    try {
        Logger::getInstance().log("Starting the Order Matching System...");

        MatchingEngine engine;
        NetworkInterface network(8080);

        Logger::getInstance().log("Initializing network interface on port 8080...");

        // Initialize critical components
        initializeSystem(engine, network);

       

        // Run network interface in a separate thread
        std::thread networkThread([&]() {
            network.receiveOrders(engine);
        });

        networkThread.join();

        Logger::getInstance().log("Shutting down the system.");
        return 0;
    } catch (const std::exception& e) {
        Logger::getInstance().log(std::string("Error: ") + e.what());
        return 1;
    }
}
