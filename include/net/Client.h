#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <thread>
#include <mutex>
#include <functional>
#include "net/GamePacket.h"

class Client {
public:
    static constexpr int BUFFER_SIZE = 1024000;

    using DataRecvListener = std::function<void(const GamePacketHead &head, const char *buffer)>;
    using ConnectStateListener = std::function<void(bool connect)>;

    Client();
    ~Client();

    bool connect(const char *serverIp, unsigned short port);
    void disconnect();

    bool isConnect() const;

    void sendData(GamePacketType type, const char *data, int size);

    void setDataRecvListener(const DataRecvListener &listener);
    void setConnectStateListener(const ConnectStateListener &listener);

private:
    void run();
    bool sendDataInternal(const char *data, int size);
    bool recvDataInternal(char *data, int size);

private:
    SOCKET mServer;
    std::mutex mSendMutex;

    bool mIsConnect;

    DataRecvListener mDataRecvListener;
    ConnectStateListener mConnectListener;
};

#endif