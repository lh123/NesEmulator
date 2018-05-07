#ifndef GAME_PROXY_H
#define GAME_PROXY_H

#include <string>
#include <functional>

#include "net/GamePacket.h"
#include "net/Server.h"
#include "net/Client.h"

enum class GameMode { Host, Client };

class GameProxy {
public:
    using FrameListener = std::function<void(Frame frame)>;
    using KeyListener = std::function<void(Button button, bool pressed)>;

    static constexpr int DEFAULT_FRAME_SKIP = 2;

    GameProxy(GameMode mode);
    ~GameProxy();

    bool startServer(unsigned short port);
    void stopServer();

    bool connectTo(std::string ip, unsigned short port);
    void disconnect();

    void sendKeyInfoToServer(Button button, bool pressed);
    void sendFrameInfoToServer(const Frame *frame);

    void setOnFrameListener(FrameListener listener);
    void setOnKeyListener(KeyListener listener);

    GameMode currentMode() const;

private:
    void handleDataRecv(const char *data, int size);

    void handleGameFrame(const char *data, int size);
    void handleGameKey(const char *data, int size);
    void handleGameAudio(const char *data, int size);

private:
    GameMode mMode;

    std::string mServerIp;
    unsigned short mServerPort;

    Client *mClient;
    Server *mServer;

    GamePacketHead mPacketHead;
    bool mHaveRecvPacketHead;

    FrameListener mFrameListener;
    KeyListener mKeyListener;

    bool mKeyBuffer[8];

    int mFrameSkip;
};

#endif