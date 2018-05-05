#include "ui/Window.h"
#include <iostream>

#include "net/Server.h"
#include "net/Client.h"

int main(int argc, char *argv[]) {
    Window window;
    if (!window.init("NES")) {
        std::printf("window init error\n");
        return 0;
    }
    window.run();
    window.close();

    // int mode = 0;
    // std::cin >> mode;
    // if (mode == 0) {
    //     Server server;
    //     server.startServer(5000);
    //     std::cin.get();
    //     std::cin.get();
    //     server.stopServer();
    //     while(1) {}
    // } else {
    //     Client client;
    //     if (client.connect("127.0.0.1", 5000)) {
    //         client.setDataRecvListener([](const char *data, int size) { std::cout << data << std::endl; });
    //         while (1) {
    //             // std::cout << "please enter id:";
    //             int id;
    //             std::cin >> id;
    //             client.sendData(reinterpret_cast<const char *>(&id), sizeof(id));
    //             // std::cout << "please enter msg:\n";
    //             std::string input;
    //             std::cin >> input;
    //             client.sendData(reinterpret_cast<const char *>(input.c_str()), input.size() + 1);
    //         }
    //     }
    // }
    return 0;
}