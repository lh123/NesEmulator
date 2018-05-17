#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "nes/Console.h"
#include "nes/Event.h"
#include "nes/Serialize.hpp"

#include <string>
#include <functional>
#include <mutex>
#include <queue>
#include <iostream>

class GameManager {
public:
    using FrameListener = std::function<void(const Frame *)>;
    using SaveStateCallBack = std::function<void(const Serialize &)>;

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

    void saveState(const SaveStateCallBack &callback);
    void loadState(const Serialize &state);

private:
    void pushEvent(const Event &event);
    Event popEvent();
    bool haveEvent();

    void handleGameThread();

    void dispatchEvent();
    void processKeyEvent(const KeyEvent &event);
    void processSaveStateEvent(const SaveStateEvent &event);
    void processLoadStateEvent(const LoadStateEvent &event);
    void processGameRunStateEvent(const GameRunStateEvent &event);

private:
    std::string mGameName;

    FrameListener mFrameListener;
    SaveStateCallBack mSaveStateCallBack;

    Console *mConsole;
    bool mRunning;
    bool mPause;

    std::mutex mEventQueueMutex;
    std::queue<Event> mEventQueue;

    bool mPlayOneKeyBuffer[8];
    bool mPlayTwoKeyBuffer[8];

    Serialize mSyncStateBuffer;
    std::mutex mSyncStateMutex;
};

#endif