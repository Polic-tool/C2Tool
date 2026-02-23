#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")


class Network {
public:
    Network();
    ~Network();

    
    bool Initialize();

    
    void Cleanup();

    
    SOCKET CreateServerSocket(int port);

    
    SOCKET AcceptClient(SOCKET serverSocket);

    
    SOCKET ConnectToServer(const std::string& host, int port);

    
    bool Send(SOCKET sock, const std::vector<uint8_t>& data);

    
    std::vector<uint8_t> Receive(SOCKET sock, size_t maxSize = 4096);

    
    void CloseSocket(SOCKET sock);

    
    bool SetNonBlocking(SOCKET sock, bool nonBlocking);

private:
    bool initialized_;
};
