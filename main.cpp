#include "Logger.h"
#include "NetworkInterface.h"
#include "MatchingEngine.h"
#include <thread>

int main() {
    try {
        Logger::getInstance().log("Starting the Order Matching System...");

        MatchingEngine engine;
        NetworkInterface network(8080);

        Logger::getInstance().log("Initializing network interface on port 8080...");

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
