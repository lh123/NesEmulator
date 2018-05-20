#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include "nes/Console.h"
#include "nes/Event.h"
#include "nes/Serialize.hpp"

#include <string>
#include <mutex>
#include <queue>
#include <iostream>

class GameManager {
public:
    using FrameListener = void (*)(void *userData, const Frame *frame);
    static constexpr char MAGIC_STR[] = "NES STATE FILE";

    GameManager();
    ~GameManager();

    bool startGame(std::string path);
    void pause();
    void resume();
    void stop();

    void reset();

    bool isPause() const;
    bool isStop() const;

    void setKeyPressed(int player, Button key, bool pressed);

    void setOnFrameListener(void *userData, const FrameListener &listener);

    void setOpenAudio(bool open);

    AudioBuffer *getAudioBuffer() const;

    void saveState(void *userData, SaveStateEvent::SaveStateCallBack callback);
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
    void *mUserData;

    Console *mConsole;
    bool mRunning;
    bool mPause;

    std::mutex mEventQueueMutex;
    std::queue<Event> mEventQueue;

    Serialize mSyncStateBuffer;
    std::mutex mSyncStateMutex;
};

#endif