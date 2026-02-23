#include "Controller.h"
#include <iostream>

int main(int argc, char* argv[]) {
    std::string serverHost = "127.0.0.1";
    int serverPort = 8888;

    if (argc > 1) {
        serverHost = argv[1];
    }
    if (argc > 2) {
        serverPort = std::atoi(argv[2]);
    }

    Controller controller(serverHost, serverPort);
    if (!controller.Connect()) {
        std::cerr << "Failed to connect to server" << std::endl;
        return 1;
    }

    std::cout << "Connected to server at " << serverHost << ":" << serverPort << std::endl;

    controller.RunInteractive();
    controller.Disconnect();

    return 0;
}
