#include "net/Server.h"
#include <iostream>
#include <thread>
#include <cstring>

Server::Server() : mRunning(false), mServer(INVALID_SOCKET), mClient(INVALID_SOCKET), mClientConnected(false) {}

Server::~Server() {
    if (mRunning) {
        stopServer();
    }
}

bool Server::startServer(unsigned short port) {
    if (mRunning) {
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
    sin.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
    if (::bind(mServer, (sockaddr *)&sin, sizeof(sockaddr_in)) == SOCKET_ERROR) {
        std::cout << "bind failed!\n";
        ::closesocket(mServer);
        ::WSACleanup();
        return false;
    }
    if (::listen(mServer, 5) == SOCKET_ERROR) {
        std::cout << "listen failed\n";
        ::closesocket(mServer);
        ::WSACleanup();
    }
    mRunning = true;
    std::thread acceptThread([this]() { handleAcceptThread(); });
    acceptThread.detach();
    std::cout << "server start\n";
    return true;
}

void Server::stopServer() {
    if (mClientConnected) {
        mClientConnected = false;
        mClient = INVALID_SOCKET;
        ::closesocket(mClient);
    }
    ::closesocket(mServer);
    ::WSACleanup();
    mRunning = false;
    std::cout << "server stop\n";
}

bool Server::isRunning() const { return mRunning; }

void Server::sendData(GamePacketType type, const char *data, int size) {
    if (mRunning && mClientConnected) {
        GamePacketHead head;
        head.type = type;
        head.size = size;
        sendDataInternal(mClient, reinterpret_cast<const char *>(&head), sizeof(GamePacketHead));
        sendDataInternal(mClient, data, size);
    }
}

void Server::setDataRecvListener(const DataRecvListener &listener) { mDataRecvListener = listener; }

void Server::setConnectStateListener(const ConnectStateListener &listener) { mConnectListener = listener; }

void Server::sendDataInternal(SOCKET socket, const char *data, int size) {
    int sendByteSize = 0;
    int errorTimes = 0;
    while (sendByteSize < size) {
        int ret = ::send(socket, data + sendByteSize, size - sendByteSize, 0);
        if (ret == SOCKET_ERROR) {
            errorTimes++;
            if (errorTimes >= 3) {
                std::cout << "Send Error\n";
                break;
            }
        } else {
            sendByteSize += ret;
        }
    }
}

bool Server::recvDataInternal(SOCKET socket, char *data, int size) {
    bool success = true;
    int recvByte = 0;
    int errorTimes = 0;
    while (recvByte < size) {
        int ret = ::recv(socket, data + recvByte, size - recvByte, 0);
        if (ret == SOCKET_ERROR) {
            errorTimes++;
            if (errorTimes >= 3) {
                std::cout << "Recv Error\n";
                success = false;
                break;
            }
        } else {
            recvByte += ret;
        }
    }
    return success;
}

void Server::handleAcceptThread() {
    while (mRunning) {
        sockaddr_in sin;
        int size = sizeof(sockaddr_in);
        SOCKET newSocket = ::accept(mServer, reinterpret_cast<sockaddr *>(&sin), &size);
        if (newSocket == INVALID_SOCKET) {
            mRunning = false;
        } else {
            if (!mClientConnected) {
                mClientConnected = true;
                mClient = newSocket;
                handleClientThread();
            }
        }
    }
}

void Server::handleClientThread() {
    GamePacketHead head;
    size_t bufferSize = BUFFER_SIZE;
    char *buffer = new char[bufferSize];
    if (mConnectListener != nullptr) {
        mConnectListener(true);
    }
    while (mClientConnected) {
        if (recvDataInternal(mClient, reinterpret_cast<char *>(&head), sizeof(GamePacketHead))) {
            if (head.size > bufferSize) {
                bufferSize = head.size;
                delete[] buffer;
                buffer = new char[bufferSize];
            }
            if (recvDataInternal(mClient, buffer, head.size)) {
                if (mDataRecvListener != nullptr) {
                    mDataRecvListener(head, buffer);
                }
            } else {
                mClientConnected = false;
            }
        } else {
            mClientConnected = false;
        }
    }
    delete[] buffer;

    mClientConnected = false;
    mClient = INVALID_SOCKET;
    ::closesocket(mClient);

    if (mConnectListener != nullptr) {
        mConnectListener(false);
    }
}