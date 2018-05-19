#ifndef GAME_PROXY_H
#define GAME_PROXY_H

#include <string>
#include <functional>

#include "nes/Serialize.hpp"
#include "net/GamePacket.h"
#include "net/Server.h"
#include "net/Client.h"

class GameManager;

enum class GameProxyMode { Local, Master, Slave };

class GameNetProxy {
public:
    GameNetProxy(GameManager *manager);
    ~GameNetProxy();

    bool startServer(unsigned short port);
    void stopServer();

    bool connectTo(const std::string &ip, unsigned short port);
    void disconnect();

    void setFrameSkip(int frameSkip);

    GameProxyMode currentMode() const;

    void pause();
    void resume();

    void setKeyPressed(int player, Button key, bool pressed);

    void sync();
    void syncImmediate();

private:
    void sendInternal(GamePacketType type, const char *data, size_t size);
    void sendPauseInfo();
    void sendResumeInfo();
    void sendKeyInfo(int player, Button button, bool pressed);
    void sendSyncInfo(const Serialize &state);

    void handleDataRecv(const GamePacketHead &head, const char *data);
    void handlConnectState(bool connect);

    void handleGameKey(const char *data, int size);
    void handleGamePause();
    void handleGameResume();
    void handleSync(const char *data, int size);

private:
    GameProxyMode mMode;
    GameManager *mManager;

    std::string mServerIp;
    unsigned short mServerPort;

    Client *mClient;
    Server *mServer;

    GamePacketHead mPacketHead;
    bool mHaveRecvPacketHead;

    Serialize mSyncBuffer;

    int mFrameSkipPeriod;
    int mFrameSkipCouner;
};

#endif