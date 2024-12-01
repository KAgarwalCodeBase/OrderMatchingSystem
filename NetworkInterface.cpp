#include "NetworkInterface.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

NetworkInterface::NetworkInterface(int port) {
    socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    bind(socketFd, (const sockaddr*)&serverAddr, sizeof(serverAddr));
}

void NetworkInterface::receiveOrders(MatchingEngine& engine) {
    char buffer[1024];
    sockaddr_in clientAddr;
    socklen_t len = sizeof(clientAddr);

    while (true) {
        int n = recvfrom(socketFd, buffer, sizeof(buffer), 0, (sockaddr*)&clientAddr, &len);
        buffer[n] = '\0';

        int orderId, quantity, timestamp, traderId;
        char side;
        double price;
        bool isMarketOrder;
        sscanf(buffer, "%d %c %lf %d %d %d %d", &orderId, &side, &price, &quantity, &timestamp, &traderId, &isMarketOrder);

        Order order(orderId, side, price, quantity, timestamp, traderId, isMarketOrder);
        engine.processOrder(order);

        sendto(socketFd, "Order Received", 14, 0, (const sockaddr*)&clientAddr, len);
    }
}
