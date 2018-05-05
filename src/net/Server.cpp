#include "net/Server.h"
#include <iostream>
#include <thread>
#include <cstring>

Server::Server() : mRunning(false), mGenerateIdCounter(0) {}

Server::~Server(){};

bool Server::startServer(unsigned short port) {
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

void Server::sendDataById(int clientId, const char *data, int size) {
    if (clientId < 0) {
        std::cout << "Invalid Id: " << clientId << "\n";
        return;
    }
    int index = findClientIndexById(clientId);
    if (index >= 0) {
        auto toClient = mClientList[index];
        sendData(toClient, data, size);
    }
}

void Server::sendDataToAll(const char *data, int size) {
    std::lock_guard<std::mutex> lock(mClientListMutex);
    for (auto client : mClientList) {
        sendData(client, data, size);
    }
}

void Server::sendData(const Client *client, const char *data, int size) {
    std::lock_guard<std::mutex> lock(mSendMutex);
    int sendByteSize = 0;
    int errorTimes = 0;
    while (sendByteSize < size) {
        int ret = ::send(client->socket, data + sendByteSize, size - sendByteSize, 0);
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

            std::lock_guard<std::mutex> lock(mClientListMutex);
            mClientList.push_back(newClient);

            std::thread clientThread([this, newClient]() { handleClientThread(newClient); });
            clientThread.detach();
        }
    }
}

void Server::handleClientThread(Client *client) {

    char *buffer = new char[BUFFER_SIZE];
    while (client->running && mRunning) {
        PacketHead head;
        int ret = ::recv(client->socket, reinterpret_cast<char *>(&head), sizeof(PacketHead), 0);
        if (ret == SOCKET_ERROR) {
            client->running = false;
        } else {
            // if (std::strcmp(head.magic, PacketHead::MAGIC) == 0) {
            //     ret = ::recv(client->socket, buffer, head.size, 0);
            //     if (ret == SOCKET_ERROR) {
            //         client->running = false;
            //     } else {
            //         if (head.target == PacketHead::TARGET_ALL) {
            //             sendDataToAll(reinterpret_cast<char *>(&head), sizeof(head));
            //             sendDataToAll(buffer, ret);
            //         } else {
            //             sendDataById(head.target, reinterpret_cast<char *>(&head), sizeof(head));
            //             sendDataById(head.target, buffer, ret);
            //         }
            //     }
            // }
        }
    }

    delete[] buffer;

    mClientListMutex.lock();
    for (auto iter = mClientList.begin(); iter != mClientList.end(); iter++) {
        auto temp = *iter;
        if (temp->id == client->id) {
            delete temp;
            mClientList.erase(iter);
            break;
        }
    }
    mClientListMutex.unlock();

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
