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
    //     while (1) {
    //     }
    // } else {
    //     Client client;
    //     if (client.connect("127.0.0.1", 5000)) {
    //         client.setDataRecvListener([](const char *data, int size) { std::cout << data << std::endl; });
    //         client.setClientConnectListener([](ClientInfoPacket *packet) {
    //             std::cout << "new client conncet" << std::endl;
    //             std::cout << "ip: " << packet->ip << "\nid: " << packet->clientId << std::endl;
    //         });

    //         client.setClientDisconnectListener([](ClientInfoPacket *packet) {
    //             std::cout << "client disconncet" << std::endl;
    //             std::cout << "ip: " << packet->ip << "\nid: " << packet->clientId << std::endl;
    //         });
    //         while (1) {
    //             std::string msg;
    //             std::cin >> msg;
    //             client.sendData(msg.c_str(), msg.size() + 1);
    //         }
    //     }
    // }
    return 0;
}