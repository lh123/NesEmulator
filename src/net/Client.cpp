#include "net/Client.h"
#include <iostream>
#include <cstring>

Client::Client() : mShouldDisconnect(false), mWaitToSendDataSize(0), mListener(nullptr) {
    mRecvBuffer = new char[BUFFER_SIZE];
}

Client::~Client() { delete[] mRecvBuffer; }

bool Client::connect(const char *serverIp, unsigned short port) {
    WORD sockVersion = MAKEWORD(2, 2);
    WSADATA wsaData;
    if (::WSAStartup(sockVersion, &wsaData) != 0) {
        std::cout << "WSAStartup failed!" << std::endl;
        return false;
    }

    mServer = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mServer == INVALID_SOCKET) {
        std::cout << "Create socket failed!" << std::endl;
        ::WSACleanup();
        return false;
    }

    sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.S_un.S_addr = inet_addr(serverIp);
    if (::connect(mServer, (sockaddr *)&sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
        std::cout << "connect failed!\n";
        ::closesocket(mServer);
        ::WSACleanup();
        return false;
    }
    std::thread th([this]() { run(); });
    th.detach();
    return true;
}

void Client::sendData(int target, const char *data, int size) {
    PacketHead head;
    head.target = target;
    head.size = size;
    head.time = std::chrono::system_clock::now().time_since_epoch().count();
    sendData(reinterpret_cast<const char *>(&head), sizeof(head));
    sendData(data, size);
}

void Client::sendData(const char *data, int size) {
    int sendByteSize = 0;
    int errorTimes = 0;
    while (sendByteSize < size) {
        int ret = ::send(mServer, data + sendByteSize, size - sendByteSize, 0);
        if (ret == SOCKET_ERROR) {
            errorTimes++;
            if (errorTimes >= 3) {
                std::cout << "SOCKET_ERROR\n";
                break;
            }
        } else {
            sendByteSize += ret;
        }
    }
}

void Client::setDataRecvListener(DataRecvListener listener) { mListener = listener; }

void Client::disConnect() {
    mShouldDisconnect = true;
    ::closesocket(mServer);
    ::WSACleanup();
}

void Client::run() {
    while (!mShouldDisconnect) {
        int ret = ::recv(mServer, mRecvBuffer, BUFFER_SIZE, 0);
        if (ret != SOCKET_ERROR) {
            if (mListener != nullptr) {
                mListener(mRecvBuffer, ret);
            }
        }
    }
}
