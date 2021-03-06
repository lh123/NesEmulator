#ifndef SERVER_H
#define SERVER_H

#include <winsock2.h>
#include <mutex>
#include <condition_variable>
#include <vector>

#include "net/GamePacket.h"

class Server {
public:
    static constexpr int BUFFER_SIZE = 1024000;
    using DataRecvListener = void (*)(void *userData, const GamePacketHead &head, const char *data);
    using ConnectStateListener = void (*)(void *userData, bool connect);

    Server();
    ~Server();

    bool startServer(unsigned short port);
    void stopServer();

    bool isRunning() const;

    void sendData(GamePacketType type, const char *data, int size);

    void setDataRecvListener(void *userData, DataRecvListener listener);
    void setConnectStateListener(void *userData, ConnectStateListener listener);

private:
    bool sendDataInternal(SOCKET socket, const char *data, int size);
    bool recvDataInternal(SOCKET socket, char *data, int size);

    void handleAcceptThread();
    void handleClientThread();

private:
    bool mRunning;
    SOCKET mServer;
    SOCKET mClient;
    bool mClientConnected;

    DataRecvListener mDataRecvListener;
    void *mDataRecvUserData;
    ConnectStateListener mConnectListener;
    void *mConnectStateUserData;

    std::mutex mSendMutex;
};

#endif