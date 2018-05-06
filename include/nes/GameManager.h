#ifndef GAME_MANAGER_H
#define GAME_MANAGER_H

#include <string>
#include <functional>
#include "nes/Console.h"
#include <mutex>
#include <queue>
#include <iostream>

struct Action {
    enum Type { StartGame, PauseGame, ResumeGame, Key };
    Action(Type type) : type(type) {}
    virtual ~Action() {}
    Type type;
};

struct KeyAction : public Action {
    KeyAction() : Action(Key) {}
    virtual ~KeyAction() {}
    int player;
    Button button;
    bool pressed;
};

class GameManager {
public:
    using FrameListener = std::function<void(Frame)>;

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

private:
    void handleGameThread();

    void processKeyAction(KeyAction *action);

private:
    FrameListener mFrameListener;

    Console *mConsole;
    bool mRunning;
    bool mPause;

    std::mutex mConsoleMutex;
    std::mutex mActionQueueMutex;
    std::queue<Action *> mActionQueue;

    bool mPlayOneKeyBuffer[8];
    bool mPlayTwoKeyBuffer[8];
};

#endif