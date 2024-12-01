#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H

#include "MatchingEngine.h"
#include <netinet/in.h>

class NetworkInterface {
private:
    int socketFd;
    sockaddr_in serverAddr;

public:
    NetworkInterface(int port);
    void receiveOrders(MatchingEngine& engine);
};

#endif // NETWORKINTERFACE_H
