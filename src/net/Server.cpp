#include "net/Server.h"
#include <iostream>
#include <thread>
#include <cstring>

Server::Server() : mRunning(false), mGenerateIdCounter(0) {}

Server::~Server(){};

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
    mClientListMutex.lock();
    for (auto client : mClientList) {
        client->running = false;
        ::closesocket(client->socket);
    }
    mClientListMutex.unlock();

    waitAllClientThreadQuit();

    mRunning = false;
    ::closesocket(mServer);

    waitAcceptThreadQuit();
    std::cout << mClientList.size();
    ::WSACleanup();
    std::cout << "server stop\n";
}

bool Server::isRunning() const { return mRunning; }

void Server::sendDataToOther(int clientId, const char *data, int size) {
    std::lock_guard<std::mutex> lock(mClientListMutex);
    for (auto client : mClientList) {
        if (client->id != clientId) {
            sendDataInternal(client->socket, data, size);
        }
    }
}

void Server::sendDataInternal(SOCKET socket, const char *data, int size) {
    std::lock_guard<std::mutex> lock(mSendMutex);
    int sendByteSize = 0;
    int errorTimes = 0;
    while (sendByteSize < size) {
        int ret = ::send(socket, data + sendByteSize, size - sendByteSize, 0);
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

bool Server::recvDataInternal(SOCKET socket, char *data, int size) {
    bool success = true;
    int recvByte = 0;
    int errorTimes = 0;
    while (recvByte < size) {
        int ret = ::recv(socket, data + recvByte, size - recvByte, 0);
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

int Server::findClientIndexById(int clientId) {
    std::lock_guard<std::mutex> lock(mClientListMutex);
    int foundIndex = -1;
    for (size_t i = 0; i < mClientList.size(); i++) {
        auto temp = mClientList[i];
        if (temp->id == clientId) {
            foundIndex = i;
            break;
        }
    }
    return foundIndex;
}

void Server::handleAcceptThread() {
    std::lock_guard<std::mutex> acceptThreadLock(mAcceptThreadMutex);
    while (mRunning) {
        sockaddr_in sin;
        int size = sizeof(sockaddr_in);
        SOCKET newSocket = ::accept(mServer, reinterpret_cast<sockaddr *>(&sin), &size);
        if (newSocket == INVALID_SOCKET) {
            mRunning = false;
        } else {
            Client *newClient = new Client;
            newClient->id = generateId();
            newClient->ipAddr = inet_ntoa(sin.sin_addr);
            newClient->running = true;
            newClient->socket = newSocket;

            std::cout << "client connect\n";

            mClientListMutex.lock();
            if (mClientList.size() < 2) {
                mClientList.push_back(newClient);

                mClientListMutex.unlock();

                std::thread clientThread([this, newClient]() { handleClientThread(newClient); });
                clientThread.detach();

                PacketHead head(PacketType::ClientConnect);
                ClientInfoPacket packet;
                packet.clientId = newClient->id;
                ::strcpy(packet.ip, newClient->ipAddr.c_str());
                head.size = sizeof(packet);
                sendDataToOther(newClient->id, reinterpret_cast<char *>(&head), sizeof(head));
                sendDataToOther(newClient->id, reinterpret_cast<char *>(&packet), sizeof(packet));
            } else {

                mClientListMutex.unlock();

                std::cout << "server full\n";
                ::closesocket(newSocket);
                delete newClient;
            }
        }
    }
}

void Server::handleClientThread(Client *client) {
    while (client->running && mRunning) {
        PacketHead head;
        if (recvDataInternal(client->socket, reinterpret_cast<char *>(&head), sizeof(head))) {
            if (head.magic == PacketHead::MAGIC) {
                sendDataToOther(client->id, reinterpret_cast<char *>(&head), sizeof(head));
                char *buffer = new char[head.size];
                if (recvDataInternal(client->socket, buffer, head.size)) {
                    sendDataToOther(client->id, buffer, head.size);
                } else {
                    client->running = false;
                    std::cout << "Socket Error\n";
                }
                delete[] buffer;
            } else {
                std::cout << "Invalid Head\n";
            }
        } else {
            client->running = false;
        }
    }

    mClientListMutex.lock();
    for (auto iter = mClientList.begin(); iter != mClientList.end(); iter++) {
        auto temp = *iter;
        if (temp->id == client->id) {
            mClientList.erase(iter);
            break;
        }
    }
    mClientListMutex.unlock();

    std::cout << "client disconnect\n";
    PacketHead head(PacketType::ClientDisconnect);
    ClientInfoPacket packet;
    packet.clientId = client->id;
    ::strcpy(packet.ip, client->ipAddr.c_str());
    head.size = sizeof(packet);
    sendDataToOther(client->id, reinterpret_cast<char *>(&head), sizeof(head));
    sendDataToOther(client->id, reinterpret_cast<char *>(&packet), sizeof(packet));

    ::closesocket(client->socket);
    delete client;

    notifyClientThreadQuit();
}

int Server::generateId() {
    int retId = mGenerateIdCounter;
    mGenerateIdCounter++;
    return retId;
}

int Server::getClientSize() {
    std::lock_guard<std::mutex> lock(mClientListMutex);
    return mClientList.size();
}

void Server::waitAcceptThreadQuit() { std::lock_guard<std::mutex> lock(mAcceptThreadMutex); }

void Server::waitAllClientThreadQuit() {
    std::unique_lock<std::mutex> lock(mQuitMutex);
    while (getClientSize() > 0) {
        mQuitCondition.wait(lock);
    }
}

void Server::notifyClientThreadQuit() {
    std::unique_lock<std::mutex> lock(mQuitMutex);
    mQuitCondition.notify_one();
}
