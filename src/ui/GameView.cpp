#include "ui/GameView.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "nes/Controller.h"

GameView::GameView(GameManager *manager) : UIComponent("##GameView"), gameManager(manager) {
    glGenTextures(1, &gameTexture);
    glBindTexture(GL_TEXTURE_2D, gameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

GameView::~GameView() { glDeleteTextures(1, &gameTexture); }

void GameView::onShow() {
    std::cout << "onShow" << std::endl;
    gameManager->setOnFrameListener([this](const Image *frame) {
        std::unique_lock<std::mutex> lock(mFrameMutex, std::defer_lock);
        if (lock.try_lock()) {
            frameBuffer = *frame;
        }
    });
}

void GameView::onClose() {
    gameManager->setOnFrameListener(nullptr);
    gameManager->stop();
}

void GameView::onFocusedChanged(bool focused) {
    std::cout << "onFocusedChanged:" << focused << std::endl;
    if (focused) {
        gameManager->resume();
    } else {
        gameManager->pause();
    }
}

void GameView::onBeforeRender() {
    ImGui::SetNextWindowSize(ImVec2(DEFAULT_WIDTH, DEFAULT_HEIGHT), ImGuiCond_FirstUseEver);

    auto sizeCallBack = [](ImGuiSizeCallbackData *data) {
        constexpr float ratio = DEFAULT_WIDTH / DEFAULT_HEIGHT;
        ImVec2 nextSize = data->DesiredSize;
        float nextRatio = nextSize.x / nextSize.y;
        if (nextRatio > ratio) {
            data->DesiredSize.y = nextSize.x / ratio;
        } else {
            data->DesiredSize.x = nextSize.y * ratio;
        }
    };

    ImGui::SetNextWindowSizeConstraints(ImVec2(DEFAULT_WIDTH, DEFAULT_HEIGHT), ImVec2(FLT_MAX, FLT_MAX), sizeCallBack);
}

void GameView::onRender() {
    readKeys();
    std::lock_guard<std::mutex> lock(mFrameMutex);
    glBindTexture(GL_TEXTURE_2D, gameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, frameBuffer.width(), frameBuffer.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 frameBuffer.pixel());
    ImGui::Image(reinterpret_cast<void *>(gameTexture), ImGui::GetContentRegionAvail());
}

void GameView::onAfterRender() {}

bool GameView::readKey(int key) {
    ImGuiIO &io = ImGui::GetIO();
    return io.KeysDown[key];
}

void GameView::readKeys() {
    gameManager->setKeyPressed(1, Button::A, readKey(GLFW_KEY_Z));
    gameManager->setKeyPressed(1, Button::B, readKey(GLFW_KEY_X));
    gameManager->setKeyPressed(1, Button::Select, readKey(GLFW_KEY_S));
    gameManager->setKeyPressed(1, Button::Start, readKey(GLFW_KEY_ENTER));
    gameManager->setKeyPressed(1, Button::Up, readKey(GLFW_KEY_UP));
    gameManager->setKeyPressed(1, Button::Down, readKey(GLFW_KEY_DOWN));
    gameManager->setKeyPressed(1, Button::Left, readKey(GLFW_KEY_LEFT));
    gameManager->setKeyPressed(1, Button::Right, readKey(GLFW_KEY_RIGHT));
}