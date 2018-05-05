#ifndef CLIENT_H
#define CLIENT_H

#include <winsock2.h>
#include <thread>
#include <mutex>
#include <functional>
#include "net/PacketHead.h"

class Client {
public:
    static constexpr int BUFFER_SIZE = 1024;

    using DataRecvListener = std::function<void(const char *buffer, int size)>;

    Client();
    ~Client();

    bool connect(const char *serverIp, unsigned short port);
    void disConnect();
    void sendData(int target, const char *data, int size);

    void setDataRecvListener(DataRecvListener listener);

private:
    void run();
    void sendData(const char *data, int size);

private:
    SOCKET mServer;
    std::mutex mLock;

    bool mShouldDisconnect;
    int mWaitToSendDataSize;

    char *mRecvBuffer;

    DataRecvListener mListener;
};

#endif