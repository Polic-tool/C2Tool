#pragma once
#include "Network.h"
#include "Protocol.h"
#include <string>
#include <thread>
#include <atomic>


class Controller {
public:
    Controller(const std::string& serverHost, int serverPort);
    ~Controller();

    
    bool Connect();

    
    void Disconnect();

    
    bool SendCommand(const std::string& clientId, const std::string& command);

    
    void ListClients();

    
    void RunInteractive();

private:
    void ReceiveLoop();
    void ProcessMessage(MessageType type, const std::vector<uint8_t>& data);

    std::string serverHost_;
    int serverPort_;
    SOCKET socket_;
    Network network_;
    std::atomic<bool> running_;
    std::thread receiveThread_;
};
