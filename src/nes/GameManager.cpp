#include "nes/GameManager.h"
#include "nes/Serialize.hpp"

#include <thread>
#include <chrono>
#include <fstream>

constexpr char GameManager::MAGIC_STR[];

GameManager::GameManager() : mRunning(false), mPause(false) { mConsole = new Console; }

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
        GameRunStateEvent event;
        event.pause = true;
        event.running = mRunning;
        event.reset = false;
        pushEvent(event);
    }
}

void GameManager::resume() {
    if (mPause && mRunning) {
        GameRunStateEvent event;
        event.pause = false;
        event.running = mRunning;
        event.reset = false;
        pushEvent(event);
    }
}

void GameManager::stop() {
    if (mRunning) {
        GameRunStateEvent event;
        event.pause = mPause;
        event.running = false;
        event.reset = false;
        pushEvent(event);
    }
}

void GameManager::reset() {
    if (mRunning) {
        GameRunStateEvent event;
        event.pause = mPause;
        event.running = mRunning;
        event.reset = true;
        pushEvent(event);
    }
}

bool GameManager::isPause() const { return mPause; }

bool GameManager::isStop() const { return !mRunning; }

void GameManager::setKeyPressed(int player, Button button, bool pressed) {
    if (player > 2) {
        return;
    }
    if (mRunning && !mPause) {
        KeyEvent event;
        event.player = player;
        event.button = button;
        event.pressed = pressed;
        pushEvent(event);
    }
}

void GameManager::setOnFrameListener(void *userData, const FrameListener &listener) {
    mUserData = userData;
    mFrameListener = listener;
}

AudioBuffer *GameManager::getAudioBuffer() const {
    if (mRunning) {
        return mConsole->getAudioBuffer();
    } else {
        return nullptr;
    }
}

void GameManager::saveState(void *userData, SaveStateEvent::SaveStateCallBack callback) {
    SaveStateEvent event;
    event.userData = userData;
    event.callback = callback;
    pushEvent(event);
}

void GameManager::loadState(const Serialize &state) {
    LoadStateEvent event;
    Serialize *loadState = new Serialize(state);
    event.state = loadState;
    pushEvent(event);
}

void GameManager::handleGameThread() {
    auto lastTime = std::chrono::system_clock::now();
    auto frameLastTime = lastTime;

    while (mRunning) {
        auto currentTime = std::chrono::system_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        auto frameDeltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - frameLastTime).count();
        lastTime = currentTime;

        dispatchEvent();

        if (!mPause) {
            mConsole->stepSeconds(deltaTime / 1000.0f);
        }

        if (frameDeltaTime >= 1000 / 60) {
            if (mFrameListener != nullptr) {
                mFrameListener(mUserData, mConsole->buffer());
            }
            frameLastTime = currentTime;
        }
        if (mRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
}

void GameManager::dispatchEvent() {
    if (haveEvent()) {
        Event e = popEvent();
        if (e != nullptr) {
            if (e.getType() == Event::Type::KeyEvent) {
                processKeyEvent(static_cast<KeyEvent &>(e));
            } else if (e.getType() == Event::Type::SaveStateEvent) {
                processSaveStateEvent(static_cast<SaveStateEvent &>(e));
            } else if (e.getType() == Event::Type::LoadStateEvent) {
                processLoadStateEvent(static_cast<LoadStateEvent &>(e));
            } else if (e.getType() == Event::Type::GameRunStateEvent) {
                processGameRunStateEvent(static_cast<GameRunStateEvent &>(e));
            }
        }
    }
}

void GameManager::processKeyEvent(const KeyEvent &event) { mConsole->setPressed(event.player, event.button, event.pressed); }

void GameManager::processSaveStateEvent(const SaveStateEvent &event) {
    Serialize saveSerialize;
    saveSerialize.writeArray(MAGIC_STR, sizeof(MAGIC_STR));
    mConsole->save(saveSerialize);
    if (event.callback != nullptr) {
        event.callback(event.userData, saveSerialize);
    }
}

void GameManager::processLoadStateEvent(const LoadStateEvent &event) {
    char magic[sizeof(MAGIC_STR)] = {0};
    Serialize &serialize = *event.state;
    serialize.readArray(magic, sizeof(magic));
    if (strcmp(magic, MAGIC_STR) == 0) {
        mConsole->load(serialize);
    } else {
        std::cout << "invalid state" << std::endl;
    }
    delete event.state;
}

void GameManager::processGameRunStateEvent(const GameRunStateEvent &event) {
    mRunning = event.running;
    mPause = event.pause;
    if (event.reset) {
        mConsole->reset();
    }
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