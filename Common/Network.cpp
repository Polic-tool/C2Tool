#include "Network.h"
#include <iostream>

Network::Network() : initialized_(false) {}

Network::~Network() {
    if (initialized_) {
        Cleanup();
    }
}

bool Network::Initialize() {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return false;
    }
    initialized_ = true;
    return true;
}

void Network::Cleanup() {
    if (initialized_) {
        WSACleanup();
        initialized_ = false;
    }
}

SOCKET Network::CreateServerSocket(int port) {
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        return INVALID_SOCKET;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSocket);
        return INVALID_SOCKET;
    }

    return serverSocket;
}

SOCKET Network::AcceptClient(SOCKET serverSocket) {
    return accept(serverSocket, nullptr, nullptr);
}

SOCKET Network::ConnectToServer(const std::string& host, int port) {
    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (clientSocket == INVALID_SOCKET) {
        return INVALID_SOCKET;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, host.c_str(), &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(clientSocket);
        return INVALID_SOCKET;
    }

    return clientSocket;
}

bool Network::Send(SOCKET sock, const std::vector<uint8_t>& data) {
    size_t totalSent = 0;
    while (totalSent < data.size()) {
        int sent = send(sock, reinterpret_cast<const char*>(data.data() + totalSent),
            static_cast<int>(data.size() - totalSent), 0);
        if (sent == SOCKET_ERROR) {
            return false;
        }
        totalSent += sent;
    }
    return true;
}

std::vector<uint8_t> Network::Receive(SOCKET sock, size_t maxSize) {
    std::vector<uint8_t> buffer(maxSize);
    int received = recv(sock, reinterpret_cast<char*>(buffer.data()), static_cast<int>(maxSize), 0);
    if (received <= 0) {
        return std::vector<uint8_t>();
    }
    buffer.resize(received);
    return buffer;
}

void Network::CloseSocket(SOCKET sock) {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
    }
}

bool Network::SetNonBlocking(SOCKET sock, bool nonBlocking) {
    u_long mode = nonBlocking ? 1 : 0;
    return ioctlsocket(sock, FIONBIO, &mode) == 0;
}
