#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <thread>
#include <mutex>
#include <functional>
#include "net/Packet.h"

class Client {
public:
    static constexpr int BUFFER_SIZE = 1024000;

    using DataRecvListener = std::function<void(const char *buffer, int size)>;
    using ClientConnectListener = std::function<void(ClientInfoPacket *packet)>;
    using ClientDisconnectListener = std::function<void(ClientInfoPacket *packet)>;

    Client();
    ~Client();

    bool connect(const char *serverIp, unsigned short port);
    void disconnect();

    bool isConnect() const;

    void sendData(const char *data, int size);

    void setDataRecvListener(DataRecvListener listener);
    void setClientConnectListener(ClientConnectListener listener);
    void setClientDisconnectListener(ClientConnectListener listener);

private:
    void run();
    bool sendDataInternal(const char *data, int size);
    bool recvDataInternal(char *data, int size);

private:
    SOCKET mServer;
    std::mutex mSendMutex;

    bool mShouldDisconnect;
    bool mIsConnect;

    DataRecvListener mDataRecvListener;
    ClientConnectListener mClienConnectListener;
    ClientDisconnectListener mClienDisconnectListener;
};

#endif