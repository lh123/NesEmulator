#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "net/PacketHead.h"

class Server {
public:
    static constexpr int BUFFER_SIZE = 1024;

    struct Client {
        int id;
        SOCKET socket;
        std::string ipAddr;
        bool running;
    };

    Server();
    ~Server();

    bool startServer(unsigned short port);
    void stopServer();

private:
    void sendDataById(int clientId, const char *data, int size);
    void sendDataToAll(const char *data, int size);
    void sendData(const Client *client, const char *data, int size);

    int findClientIndexById(int clientId);

    void handleAcceptThread();
    void handleClientThread(Client *client);

    int generateId();
    int getClientSize();

    void waitAcceptThreadQuit();

    void waitAllClientThreadQuit();
    void notifyClientThreadQuit();

private:
    bool mRunning;

    int mGenerateIdCounter;

    SOCKET mServer;
    std::vector<Client *> mClientList;
    std::mutex mClientListMutex;

    std::mutex mAcceptThreadMutex;
    std::mutex mQuitMutex;
    std::condition_variable mQuitCondition;

    std::mutex mSendMutex;
};

#endif