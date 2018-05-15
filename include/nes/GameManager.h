#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "nes/Console.h"
#include "nes/Event.h"

#include <string>
#include <functional>
#include <mutex>
#include <queue>
#include <iostream>

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

    void setOnFrameListener(const FrameListener &listener);

    void setOpenAudio(bool open);

    AudioBuffer *getAudioBuffer() const;

    bool saveState();
    bool loadState();

private:
    void pushEvent(const Event &event);
    Event popEvent();
    bool haveEvent();

    void handleGameThread();

    void processKeyEvent(const KeyEvent &event);
    void processSaveStateEvent(const SaveStateEvent &event);
    void processLoadStateEvent(const LoadStateEvent &event);
    void processGameStateEvent(const GameStateEvent &event);

private:
    std::string mGameName;

    FrameListener mFrameListener;

    Console *mConsole;
    bool mRunning;
    bool mPause;

    std::mutex mEventQueueMutex;
    std::queue<Event> mEventQueue;

    bool mPlayOneKeyBuffer[8];
    bool mPlayTwoKeyBuffer[8];
};

#endif