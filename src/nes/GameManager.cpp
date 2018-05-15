#include "nes/GameManager.h"
#include "nes/Serialize.hpp"

#include <thread>
#include <chrono>
#include <fstream>

GameManager::GameManager() : mRunning(false), mPause(false), mPlayOneKeyBuffer{false}, mPlayTwoKeyBuffer{false} {
    mConsole = new Console;
}

GameManager::~GameManager() { stop(); }

bool GameManager::startGame(std::string path) {
    if (!mRunning) {
        if (mConsole->loadRom(path)) {
            mRunning = true;
            mPause = false;
            std::thread gameThread([this]() { handleGameThread(); });
            gameThread.detach();
        } else {
            mRunning = false;
        }
    }
    return mRunning;
}

void GameManager::pause() {
    if (!mPause && mRunning) {
        GameStateEvent event;
        event.pause = true;
        event.running = mRunning;
        pushEvent(event);
    }
}

void GameManager::resume() {
    if (mPause && mRunning) {
        GameStateEvent event;
        event.pause = false;
        event.running = mRunning;
        pushEvent(event);
    }
}

void GameManager::stop() {
    if (mRunning) {
        GameStateEvent event;
        event.pause = mPause;
        event.running = false;
        pushEvent(event);
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
        KeyEvent event;
        event.player = player;
        event.button = button;
        event.pressed = pressed;
        pushEvent(event);
    }
}

void GameManager::setOnFrameListener(const FrameListener &listener) { mFrameListener = listener; }

AudioBuffer *GameManager::getAudioBuffer() const {
    if (mRunning) {
        return mConsole->getAudioBuffer();
    } else {
        return nullptr;
    }
}

bool GameManager::saveState() {
    if (!mRunning) {
        return false;
    }
    SaveStateEvent event;
    strcpy(event.fileName, "state.sav");
    pushEvent(event);
    return true;
}

bool GameManager::loadState() {
    if (!mRunning) {
        return false;
    }
    LoadStateEvent event;
    strcpy(event.fileName, "state.sav");
    pushEvent(event);
    return true;
}

void GameManager::handleGameThread() {
    auto lastTime = std::chrono::system_clock::now();
    auto frameLastTime = lastTime;

    while (mRunning) {
        auto currentTime = std::chrono::system_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        auto frameDeltaTime =
            std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - frameLastTime).count();
        lastTime = currentTime;

        if (haveEvent()) {
            Event e = popEvent();
            if (e != nullptr) {
                if (e.getType() == Event::Type::KeyEvent) {
                    processKeyEvent(e);
                } else if (e.getType() == Event::Type::SaveStateEvent) {
                    processSaveStateEvent(e);
                } else if (e.getType() == Event::Type::LoadStateEvent) {
                    processLoadStateEvent(e);
                } else if (e.getType() == Event::Type::GameStateEvent) {
                    processGameStateEvent(e);
                }
            }
        }
        if (!mPause) {
            mConsole->stepSeconds(deltaTime / 1000.0f);
        }

        if (frameDeltaTime >= 1000 / 60) {
            if (mFrameListener != nullptr) {
                mFrameListener(mConsole->buffer());
            }
            frameLastTime = currentTime;
        }
        if (mRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void GameManager::processKeyEvent(const KeyEvent &event) {
    mConsole->setPressed(event.player, event.button, event.pressed);
}

void GameManager::processSaveStateEvent(const SaveStateEvent &event) {
    std::ofstream stateFile(event.fileName, std::ios::binary | std::ios::out);
    if (stateFile.is_open()) {
        Serialize serialize;
        mConsole->save(serialize);
        serialize.writeToStream(stateFile);
    }
}

void GameManager::processLoadStateEvent(const LoadStateEvent &event) {
    std::ifstream stateFile(event.fileName, std::ios::binary | std::ios::in);
    if (stateFile.is_open()) {
        Serialize serialize;
        serialize.readFromStream(stateFile);
        mConsole->load(serialize);
    }
}

void GameManager::processGameStateEvent(const GameStateEvent &event) {
    mRunning = event.running;
    mPause = event.pause;
}

void GameManager::pushEvent(const Event &event) {
    std::lock_guard<std::mutex> lock(mEventQueueMutex);
    mEventQueue.push(event);
}

Event GameManager::popEvent() {
    std::lock_guard<std::mutex> lock(mEventQueueMutex);
    Event event = mEventQueue.front();
    mEventQueue.pop();
    return event;
}

bool GameManager::haveEvent() {
    std::lock_guard<std::mutex> lock(mEventQueueMutex);
    return !mEventQueue.empty();
}