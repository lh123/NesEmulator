#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <string>
#include <functional>
#include "nes/Console.h"
#include <mutex>
#include <queue>
#include <iostream>

#include "nes/Pool.hpp"

struct KeyAction {
    int player;
    Button button;
    bool pressed;
};

class GameManager {
public:
    using FrameListener = std::function<void(const Frame *)>;

    GameManager();
    ~GameManager();

    bool startGame(std::string path);
    void pause();
    void resume();
    void stop();

    bool isPause() const;
    bool isStop() const;

    void setKeyPressed(int player, Button key, bool pressed);

    void setOnFrameListener(FrameListener listener);

    void setOpenAudio(bool open);

    AudioBuffer *getAudioBuffer() const;

    bool saveState();
    bool loadState();

private:
    void handleGameThread();

    void processKeyAction(KeyAction *action);

private:
    std::string mGameName;

    FrameListener mFrameListener;

    Console *mConsole;
    bool mRunning;
    bool mPause;

    std::mutex mConsoleMutex;
    std::mutex mActionQueueMutex;
    std::queue<KeyAction *> mActionQueue;

    Pool<KeyAction> mKeyActionPool;

    bool mPlayOneKeyBuffer[8];
    bool mPlayTwoKeyBuffer[8];
};

#endif