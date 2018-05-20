#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <thread>
#include <mutex>
#include "net/GamePacket.h"

class Client {
public:
    static constexpr int BUFFER_SIZE = 1024000;

    using DataRecvListener = void (*)(void *userData, const GamePacketHead &head, const char *buffer);
    using ConnectStateListener = void (*)(void *userData, bool connect);

    Client();
    ~Client();

    bool connect(const char *serverIp, unsigned short port);
    void disconnect();

    bool isConnect() const;

    void sendData(GamePacketType type, const char *data, int size);

    void setDataRecvListener(void *userData, DataRecvListener listener);
    void setConnectStateListener(void *userData, ConnectStateListener listener);

private:
    void run();
    bool sendDataInternal(const char *data, int size);
    bool recvDataInternal(char *data, int size);

private:
    SOCKET mServer;
    std::mutex mSendMutex;

    bool mIsConnect;

    DataRecvListener mDataRecvListener;
    void *mDataRecvUserData;

    ConnectStateListener mConnectListener;
    void *mConnectStateUserData;
};

#endif