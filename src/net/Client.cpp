#include "net/Client.h"
#include <iostream>
#include <cstring>

Client::Client() : mIsConnect(false), mDataRecvListener(nullptr), mConnectListener(nullptr) {}

Client::~Client() {
    if (mIsConnect) {
        disconnect();
    }
}

bool Client::connect(const char *serverIp, unsigned short port) {
    if (mIsConnect) {
        return false;
    }
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
    mIsConnect = true;
    std::thread th([this]() { run(); });
    th.detach();
    return true;
}

void Client::sendData(GamePacketType type, const char *data, int size) {
    if (mIsConnect) {
        GamePacketHead head;
        head.type = type;
        head.size = size;
        sendDataInternal(reinterpret_cast<const char *>(&head), sizeof(GamePacketHead));
        sendDataInternal(data, size);
    }
}

bool Client::sendDataInternal(const char *data, int size) {
    std::lock_guard<std::mutex> lock(mSendMutex);
    bool success = true;
    int sendByteSize = 0;
    int errorTimes = 0;
    while (sendByteSize < size) {
        int ret = ::send(mServer, data + sendByteSize, size - sendByteSize, 0);
        if (ret == SOCKET_ERROR) {
            errorTimes++;
            if (errorTimes >= 3) {
                success = false;
                break;
            }
        } else {
            sendByteSize += ret;
        }
    }
    return success;
}

bool Client::recvDataInternal(char *data, int size) {
    bool success = true;
    int recvByte = 0;
    int errorTimes = 0;
    while (recvByte < size) {
        int ret = ::recv(mServer, data + recvByte, size - recvByte, 0);
        if (ret == SOCKET_ERROR) {
            errorTimes++;
            if (errorTimes >= 3) {
                success = false;
                break;
            }
        } else {
            recvByte += ret;
        }
    }
    return success;
}

void Client::setDataRecvListener(void *userData, DataRecvListener listener) {
    mDataRecvUserData = userData;
    mDataRecvListener = listener;
}

void Client::setConnectStateListener(void *userData, ConnectStateListener listener) {
    mConnectStateUserData = userData;
    mConnectListener = listener;
}

void Client::disconnect() {
    if (mIsConnect) {
        mIsConnect = false;
        ::closesocket(mServer);
        ::WSACleanup();
    }
}

bool Client::isConnect() const { return mIsConnect; }

void Client::run() {
    if (mConnectListener != nullptr) {
        mConnectListener(mConnectStateUserData, true);
    }
    size_t bufferSize = BUFFER_SIZE;
    char *buffer = new char[bufferSize];
    GamePacketHead head;
    while (mIsConnect) {
        if (recvDataInternal(reinterpret_cast<char *>(&head), sizeof(GamePacketHead))) {
            if (head.size > bufferSize) {
                bufferSize = head.size;
                delete[] buffer;
                buffer = new char[bufferSize];
            }
            if (recvDataInternal(buffer, head.size)) {
                if (mDataRecvListener != nullptr) {
                    mDataRecvListener(mDataRecvUserData, head, buffer);
                }
            } else {
                disconnect();
            }
        } else {
            disconnect();
        }
    }
    delete[] buffer;
    if (mConnectListener != nullptr) {
        mConnectListener(mConnectStateUserData, false);
    }
}
