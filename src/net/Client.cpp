#include "net/Client.h"
#include <iostream>
#include <cstring>

Client::Client()
    : mShouldDisconnect(false), mIsConnect(false), mDataRecvListener(nullptr), mClienConnectListener(nullptr) {}

Client::~Client() {}

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
    std::thread th([this]() { run(); });
    th.detach();
    mIsConnect = true;
    return true;
}

void Client::sendData(const char *data, int size) {
    if (mIsConnect) {
        PacketHead head(PacketType::Data);
        head.size = size;
        sendDataInternal(reinterpret_cast<const char *>(&head), sizeof(head));
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
                std::cout << "SOCKET_ERROR\n";
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
                std::cout << "SOCKET_ERROR\n";
                success = false;
                break;
            }
        } else {
            recvByte += ret;
        }
    }
    return success;
}

void Client::setDataRecvListener(DataRecvListener listener) { mDataRecvListener = listener; }

void Client::setClientConnectListener(ClientConnectListener listener) { mClienConnectListener = listener; }

void Client::setClientDisconnectListener(ClientDisconnectListener listener) { mClienDisconnectListener = listener; }

void Client::disconnect() {
    if (mIsConnect) {
        mIsConnect = true;
        mShouldDisconnect = true;
        ::closesocket(mServer);
        ::WSACleanup();
    }
}

bool Client::isConnect() const { return mIsConnect; }

void Client::run() {
    PacketHead head;
    while (!mShouldDisconnect) {
        if (recvDataInternal(reinterpret_cast<char *>(&head), sizeof(PacketHead))) {
            if (head.magic == PacketHead::MAGIC) {
                if (head.type == PacketType::ClientConnect) {
                    ClientInfoPacket packet;
                    if (recvDataInternal(reinterpret_cast<char *>(&packet), head.size)) {
                        if (mClienConnectListener != nullptr) {
                            mClienConnectListener(&packet);
                        }
                    } else {
                        disconnect();
                    }
                } else if (head.type == PacketType::ClientDisconnect) {
                    ClientInfoPacket packet;
                    if (recvDataInternal(reinterpret_cast<char *>(&packet), head.size)) {
                        if (mClienDisconnectListener != nullptr) {
                            mClienDisconnectListener(&packet);
                        }
                    } else {
                        disconnect();
                    }
                } else if (head.type == PacketType::Data) {
                    char *buffer = new char[head.size];
                    if (recvDataInternal(buffer, head.size)) {
                        if (mDataRecvListener != nullptr) {
                            mDataRecvListener(buffer, head.size);
                        }
                    } else {
                        disconnect();
                    }
                    delete[] buffer;
                } else {
                    std::cout << "Invalid Packet Type\n";
                }
            } else {
                std::cout << "Invalid Packet Head\n";
            }
        } else {
            disconnect();
        }
    }
}
