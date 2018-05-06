#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "net/Packet.h"

class Server {
public:
    static constexpr int BUFFER_SIZE = 1024000;

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

    bool isRunning() const;

private:
    void sendDataToOther(int clientId, const char *data, int size);

    void sendDataInternal(SOCKET socket, const char *data, int size);
    bool recvDataInternal(SOCKET socket, char *data, int size);
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