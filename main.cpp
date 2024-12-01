#include "NetworkInterface.h"
#include "MatchingEngine.h"
#include <thread>

int main() {
    MatchingEngine engine;
    NetworkInterface network(8080);

    std::thread networkThread([&]() { network.receiveOrders(engine); });

    networkThread.join();
    return 0;
}
