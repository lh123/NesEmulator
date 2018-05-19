#include "net/GameNetProxy.h"
#include <iostream>
#include <string.h>

#include "nes/GameManager.h"

GameNetProxy::GameNetProxy(GameManager *manager)
    : mMode(GameProxyMode::Local), mManager(manager), mHaveRecvPacketHead(false), mFrameSkipPeriod(0), mFrameSkipCouner(0) {
    mServer = new Server;
    mClient = new Client;

    auto connectListener = [this](bool connect) { this->handlConnectState(connect); };
    auto dataListener = [this](const GamePacketHead &head, const char *data) { this->handleDataRecv(head, data); };

    mServer->setConnectStateListener(connectListener);
    mServer->setDataRecvListener(dataListener);

    mClient->setConnectStateListener(connectListener);
    mClient->setDataRecvListener(dataListener);
}

GameNetProxy::~GameNetProxy() {
    if (mMode == GameProxyMode::Master) {
        stopServer();
    } else if (mMode == GameProxyMode::Slave) {
        disconnect();
    }
    delete mServer;
    delete mClient;
}

bool GameNetProxy::startServer(unsigned short port) {
    if (mMode != GameProxyMode::Local) {
        return false;
    }
    mServerPort = port;
    mServerIp = "127.0.0.1";
    if (mServer->startServer(mServerPort)) {
        mMode = GameProxyMode::Master;
        return true;
    } else {
        return false;
    }
}

void GameNetProxy::stopServer() {
    if (mMode == GameProxyMode::Master) {
        if (mServer->isRunning()) {
            mServer->stopServer();
        }
        mMode = GameProxyMode::Local;
    }
}

bool GameNetProxy::connectTo(const std::string &ip, unsigned short port) {
    if (mMode != GameProxyMode::Local) {
        return false;
    }
    mServerIp = ip;
    mServerPort = port;
    if (mClient->connect(mServerIp.c_str(), mServerPort)) {
        mMode = GameProxyMode::Slave;
        return true;
    } else {
        return false;
    }
}

void GameNetProxy::disconnect() {
    if (mMode == GameProxyMode::Slave) {
        if (mClient->isConnect()) {
            mClient->disconnect();
        }
        mMode = GameProxyMode::Local;
    }
}

void GameNetProxy::setFrameSkip(int frameSkip) { mFrameSkipPeriod = frameSkip; }

GameProxyMode GameNetProxy::currentMode() const { return mMode; }

void GameNetProxy::pause() {
    if (mMode != GameProxyMode::Local) {
        sendPauseInfo();
    }
}

void GameNetProxy::resume() {
    if (mMode != GameProxyMode::Local) {
        sendResumeInfo();
    }
}

void GameNetProxy::setKeyPressed(int player, Button key, bool pressed) {
    if (mMode != GameProxyMode::Local) {
        sendKeyInfo(player, key, pressed);
    }
}

void GameNetProxy::sync() {
    if (mFrameSkipCouner == 0) {
        mFrameSkipCouner = mFrameSkipPeriod;
        syncImmediate();
    } else {
        mFrameSkipCouner--;
    }
}

void GameNetProxy::syncImmediate() {
    if (mMode == GameProxyMode::Master) {
        mManager->saveState([this](const Serialize &state) { sendSyncInfo(state); });
    }
}

void GameNetProxy::sendInternal(GamePacketType type, const char *data, size_t size) {
    if (mMode == GameProxyMode::Master) {
        mServer->sendData(type, data, size);
    } else if (mMode == GameProxyMode::Slave) {
        mClient->sendData(type, data, size);
    }
}

void GameNetProxy::sendPauseInfo() {
    GamePausePacket packet;
    sendInternal(GamePacketType::Pause, reinterpret_cast<const char *>(&packet), sizeof(GamePausePacket));
}

void GameNetProxy::sendResumeInfo() {
    GameResumePacket packet;
    sendInternal(GamePacketType::Resume, reinterpret_cast<const char *>(&packet), sizeof(GameResumePacket));
}

void GameNetProxy::sendKeyInfo(int player, Button button, bool pressed) {
    GameKeyPacket packet;
    packet.player = player;
    packet.button = button;
    packet.pressed = pressed;
    sendInternal(GamePacketType::Key, reinterpret_cast<const char *>(&packet), sizeof(GameKeyPacket));
}

void GameNetProxy::sendSyncInfo(const Serialize &state) {
    mSyncBuffer = state;
    auto data = mSyncBuffer.getData();
    sendInternal(GamePacketType::Sync, data.first, data.second);
}

void GameNetProxy::handleDataRecv(const GamePacketHead &head, const char *data) {
    switch (head.type) {
    case GamePacketType::Key:
        handleGameKey(data, head.size);
        break;
    case GamePacketType::Pause:
        handleGamePause();
        break;
    case GamePacketType::Resume:
        handleGameResume();
        break;
    case GamePacketType::Sync:
        handleSync(data, head.size);
        break;
    }
}

void GameNetProxy::handlConnectState(bool connect) {
    if (connect) {
        if (mMode == GameProxyMode::Master) {
            mManager->reset();
        }
    } else {
        if (mMode == GameProxyMode::Slave) {
            disconnect();
        }
    }
}

void GameNetProxy::handleGameKey(const char *data, int size) {
    if (size != sizeof(GameKeyPacket)) {
        std::cout << "Invalid GameKeyPacket Size" << std::endl;
        return;
    }
    GameKeyPacket packet = *reinterpret_cast<const GameKeyPacket *>(data);
    mManager->setKeyPressed(packet.player, packet.button, packet.pressed);
}

void GameNetProxy::handleSync(const char *data, int size) {
    mSyncBuffer.clear();
    mSyncBuffer.readFromMemory(data, size);
    mManager->loadState(mSyncBuffer);
}

void GameNetProxy::handleGamePause() { mManager->pause(); }

void GameNetProxy::handleGameResume() { mManager->resume(); }
