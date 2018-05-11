#include "nes/GameManager.h"
#include "nes/Serialize.hpp"

#include <thread>
#include <chrono>
#include <fstream>

GameManager::GameManager()
    : mConsole(nullptr), mRunning(false), mPause(false), mPlayOneKeyBuffer{false}, mPlayTwoKeyBuffer{false} {}

GameManager::~GameManager() { stop(); }

bool GameManager::startGame(std::string path) {
    if (!mRunning) {
        mConsole = new Console(path.c_str());
        if (!mConsole->isOpenRom()) {
            delete mConsole;
            mRunning = false;
        } else {
            mRunning = true;
            mPause = false;
            std::thread gameThread([this]() { handleGameThread(); });
            gameThread.detach();
        }
    }
    return mRunning;
}

void GameManager::pause() {
    if (!mPause && mRunning) {
        mPause = true;
        std::lock_guard<std::mutex> lock(mConsoleMutex);
    }
}

void GameManager::resume() {
    if (mPause && mRunning) {
        mPause = false;
        std::thread gameThread([this]() { handleGameThread(); });
        gameThread.detach();
    }
}

void GameManager::stop() {
    if (mRunning) {
        mRunning = false;
        mPause = true;
        std::lock_guard<std::mutex> lock(mConsoleMutex);
        delete mConsole;
    }
}

bool GameManager::isPause() const { return mPause; }

bool GameManager::isStop() const { return !mRunning; }

void GameManager::setKeyPressed(int player, Button button, bool pressed) {
    if (player > 2) {
        return;
    }
    if (player == 1) {
        if (mPlayOneKeyBuffer[(int)button] == pressed) {
            return;
        } else {
            mPlayOneKeyBuffer[(int)button] = pressed;
        }
    } else {
        if (mPlayTwoKeyBuffer[(int)button] == pressed) {
            return;
        } else {
            mPlayTwoKeyBuffer[(int)button] = pressed;
        }
    }
    if (mRunning && !mPause) {
        KeyAction *action = new KeyAction;
        action->player = player;
        action->button = button;
        action->pressed = pressed;

        std::lock_guard<std::mutex> lock(mActionQueueMutex);
        mActionQueue.push(action);
    }
}

void GameManager::setOnFrameListener(FrameListener listener) { mFrameListener = listener; }

bool GameManager::saveState() {
    if (!mRunning) {
        return false;
    }
    bool ret = false;
    pause();
    std::fstream stateFile("state.sav", std::ios::binary | std::ios::out);
    if (stateFile.is_open()) {
        Serialize state(&stateFile);
        state << 0xFFEE;
        mConsole->save(state);
        ret = true;
    } else {
        ret = false;
    }
    resume();
    return ret;
}

bool GameManager::loadState() {
    if (!mRunning) {
        return false;
    }
    bool ret = false;
    pause();
    std::fstream stateFile("state.sav", std::ios::binary | std::ios::in);
    if (stateFile.is_open()) {
        Serialize state(&stateFile);
        int magic;
        state >> magic;
        if (magic != 0xFFEE) {
            ret = false;
        } else {
            mConsole->load(state);
            ret = true;
        }
    } else {
        ret = false;
    }
    resume();
    return ret;
}

void GameManager::handleGameThread() {
    std::lock_guard<std::mutex> lock(mConsoleMutex);
    auto lastTime = std::chrono::system_clock::now();
    auto frameLastTime = lastTime;
    while (mRunning && !mPause) {
        auto currentTime = std::chrono::system_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        auto frameDeltaTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - frameLastTime).count();
        lastTime = currentTime;

        mConsole->stepSeconds(deltaTime / 1000.0f);

        Action *action = nullptr;
        mActionQueueMutex.lock();
        if (!mActionQueue.empty()) {
            action = mActionQueue.front();
            mActionQueue.pop();
        }
        mActionQueueMutex.unlock();
        if (action != nullptr) {
            if (action->type == Action::Key) {
                processKeyAction(reinterpret_cast<KeyAction *>(action));
            }
        }
        if (action != nullptr) {
            delete action;
        }

        if (frameDeltaTime >= 1000 / 60) {
            if (mFrameListener != nullptr) {
                mFrameListener(*mConsole->buffer());
            }
            frameLastTime = currentTime;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void GameManager::processKeyAction(KeyAction *action) {
    mConsole->setPressed(action->player, action->button, action->pressed);
}