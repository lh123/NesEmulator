#include "nes/GameManager.h"
#include <thread>
#include <chrono>

GameManager::GameManager()
    : mConsole(nullptr), mRunning(false), mPause(false), mPlayOneKeyBuffer{false}, mPlayTwoKeyBuffer{false} {}

GameManager::~GameManager() { stop(); }

bool GameManager::startGame(std::string path) {
    if (mConsole == nullptr) {
        mConsole = new Console(path.c_str());
        if (!mConsole->isOpenRom()) {
            delete mConsole;
            mConsole = nullptr;
            return false;
        } else {
            mRunning = true;
            mPause = false;
            std::thread gameThread([this]() { handleGameThread(); });
            gameThread.detach();
            return true;
        }
    }
    return false;
}

void GameManager::pause() {
    if (!mPause && mRunning) {
        mPause = true;
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
        if (mConsole != nullptr) {
            delete mConsole;
            mConsole = nullptr;
        }
    }
}

void GameManager::setKeyPressed(int player, Button button, bool pressed) {
    if (player > 1) {
        return;
    }
    if (player == 0) {
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
            std::cout << "proecess Action\n count: " << mActionQueue.size() << "\n";
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
                mFrameListener(mConsole->buffer());
            }
            frameLastTime = currentTime;
        }
    }
}

void GameManager::processKeyAction(KeyAction *action) {
    mConsole->setPressed(action->player, action->button, action->pressed);
}