#include "net/GameProxy.h"

#include <iostream>
#include <string.h>

GameProxy::GameProxy(GameProxyMode mode)
    : mMode(mode), mHaveRecvPacketHead(false), mKeyBuffer{false}, mFrameSkip(DEFAULT_FRAME_SKIP) {
    mServer = new Server;
    mClient = new Client;
    mClient->setDataRecvListener([this](const char *data, int size) { this->handleDataRecv(data, size); });
}

GameProxy::~GameProxy() { delete mClient; }

bool GameProxy::startServer(unsigned short port) {
    if (mMode != GameProxyMode::Host) {
        return false;
    }
    if (!mServer->isRunning()) {
        mServerPort = port;
        mServerIp = "127.0.0.1";
        if (mServer->startServer(mServerPort)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return connectTo(mServerIp, mServerPort);
        }
    } else {
        return connectTo(mServerIp, mServerPort);
    }
    return false;
}

void GameProxy::stopServer() {
    if (mMode != GameProxyMode::Host) {
        return;
    }
    if (mServer->isRunning()) {
        disconnect();
        mServer->stopServer();
    }
}

bool GameProxy::connectTo(std::string ip, unsigned short port) {
    if (mClient->isConnect()) {
        return false;
    }

    mServerIp = ip;
    mServerPort = port;

    return mClient->connect(mServerIp.c_str(), mServerPort);
}

void GameProxy::disconnect() {
    if (mClient->isConnect()) {
        mClient->disconnect();
    }
}

void GameProxy::sendKeyInfoToServer(Button button, bool pressed) {
    if (mKeyBuffer[(int)button] == pressed) {
        return;
    }
    mKeyBuffer[(int)button] = pressed;
    GamePacketHead head;
    head.type = GamePacketType::Key;
    head.size = sizeof(GameKeyPacket);

    GameKeyPacket body;
    body.button = button;
    body.pressed = pressed;

    mClient->sendData(reinterpret_cast<char *>(&head), sizeof(GamePacketHead));
    mClient->sendData(reinterpret_cast<char *>(&body), sizeof(GameKeyPacket));
}

void GameProxy::sendFrameInfoToServer(const Frame *frame) {
    if (mFrameSkip > 0) {
        mFrameSkip--;
    } else {
        mFrameSkip = DEFAULT_FRAME_SKIP;
        GamePacketHead head;
        head.type = GamePacketType::Frame;
        head.size = sizeof(GameFramePacket);

        GameFramePacket body;
        ::memcpy(body.imageData, frame->pixel(), sizeof(body.imageData));

        mClient->sendData(reinterpret_cast<char *>(&head), sizeof(GamePacketHead));
        mClient->sendData(reinterpret_cast<char *>(&body), sizeof(GameFramePacket));
    }
}

void GameProxy::setOnFrameListener(FrameListener listener) { mFrameListener = listener; }

void GameProxy::setOnKeyListener(KeyListener listener) { mKeyListener = listener; }

GameProxyMode GameProxy::currentMode() const { return mMode; }

void GameProxy::handleDataRecv(const char *data, int size) {
    if (!mHaveRecvPacketHead) {
        if (size == sizeof(GamePacketHead)) {
            mPacketHead = *reinterpret_cast<const GamePacketHead *>(data);
            mHaveRecvPacketHead = true;
        } else {
            std::cout << "Invalid Packet Head Size" << std::endl;
        }
    } else {
        switch (mPacketHead.type) {
        case GamePacketType::Frame:
            handleGameFrame(data, size);
            break;
        case GamePacketType::Key:
            handleGameKey(data, size);
            break;
        case GamePacketType::Audio:
            handleGameAudio(data, size);
            break;
        }
        mHaveRecvPacketHead = false;
    }
}

void GameProxy::handleGameFrame(const char *data, int size) {
    if (size != sizeof(GameFramePacket)) {
        std::cout << "Invalid GameFramePacket Size" << std::endl;
        return;
    }
    const GameFramePacket *packet = reinterpret_cast<const GameFramePacket *>(data);
    if (size == Frame::SIZE) {
        mFrameBuffer.setData(packet->imageData);
    }
    if (mFrameListener != nullptr) {
        mFrameListener(&mFrameBuffer);
    }
}

void GameProxy::handleGameKey(const char *data, int size) {
    if (size != sizeof(GameKeyPacket)) {
        std::cout << "Invalid GameKeyPacket Size" << std::endl;
        return;
    }
    GameKeyPacket packet = *reinterpret_cast<const GameKeyPacket *>(data);
    if (mKeyListener != nullptr) {
        mKeyListener(packet.button, packet.pressed);
    }
}

void GameProxy::handleGameAudio(const char *data, int size) {
    // TODO
}
