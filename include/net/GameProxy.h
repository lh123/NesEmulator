#ifndef GAME_PROXY_H
#define GAME_PROXY_H

#include <string>
#include <functional>

#include "nes/AudioBuffer.h"
#include "net/GamePacket.h"
#include "net/Server.h"
#include "net/Client.h"

enum class GameProxyMode { Host, Client };

class GameProxy {
public:
    using FrameListener = std::function<void(const Frame *)>;
    using KeyListener = std::function<void(Button, bool)>;

    GameProxy(GameProxyMode mode);
    ~GameProxy();

    bool startServer(unsigned short port);
    void stopServer();

    bool connectTo(std::string ip, unsigned short port);
    void disconnect();

    void sendKeyInfoToServer(Button button, bool pressed);
    void sendFrameInfoToServer(const Frame *frame);
    void sendAudioInfoToServer(const float *audioBuffer, int length);

    void setOnFrameListener(FrameListener listener);
    void setOnKeyListener(KeyListener listener);

    AudioBuffer *getAudioBuffer();

    void setFrameSkip(int frameSkip);
    void setQuality(int quality);

    GameProxyMode currentMode() const;

private:
    void handleDataRecv(const char *data, int size);

    void handleGameFrame(const char *data, int size);
    void handleGameKey(const char *data, int size);
    void handleGameAudio(const char *data, int size);

private:
    GameProxyMode mMode;

    std::string mServerIp;
    unsigned short mServerPort;

    Client *mClient;
    Server *mServer;

    GamePacketHead mPacketHead;
    bool mHaveRecvPacketHead;

    FrameListener mFrameListener;
    KeyListener mKeyListener;

    bool mKeyBuffer[8];
    Frame mFrameBuffer;
    AudioBuffer mAudioBuffer;

    int mFrameSkipPeriod;
    int mFrameSkipCouner;

    int mQuality;

    FrameCompress mFrameCompress;
};

#endif