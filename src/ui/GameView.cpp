#include "ui/GameView.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include "nes/Controller.h"

GameView::GameView()
    : UIComponent(), isPause(false), isCollapsed(false), isFocused(false), console(nullptr), gameThread(nullptr) {
    glGenTextures(1, &gameTexture);
    glBindTexture(GL_TEXTURE_2D, gameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

GameView::~GameView() {
    glDeleteTextures(1, &gameTexture);
    pause();
    destory();
}

void GameView::render() {
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
    ImGui::Begin("##GameView", &mShow);

    bool preShow = mShow;
    bool focused = ImGui::IsWindowFocused();

    Image *image = console->buffer();
    glBindTexture(GL_TEXTURE_2D, gameTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->width(), image->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 image->pixel());
    ImGui::Image(reinterpret_cast<void *>(gameTexture), ImGui::GetContentRegionAvail());

    if (focused && !isFocused) {
        // std::cout << "focused\n";
        resume();
    } else if (!focused && isFocused) {
        // std::cout << "loose focused\n";
        pause();
    }

    if (preShow && !mShow) {
        destory();
    }

    isFocused = focused;

    ImGui::End();
}

void GameView::show() {
    // std::cout << "gameview show" << std::endl;
    if (!mShow && !nesName.empty()) {
        // std::cout << "gameview open game" << std::endl;
        isPause = false;
        console = new Console(nesName.c_str());
        gameThread = new std::thread(std::bind(GameView::step, this));
        gameThread->detach();
    }
    UIComponent::show();
}

void GameView::setGamePath(const char *path) { nesName = path; }

void GameView::resume() {
    if (isPause) {
        // std::cout << "gameview resume" << std::endl;
        isPause = false;
        gameThread = new std::thread(std::bind(GameView::step, this));
        gameThread->detach();
    }
}

void GameView::pause() {
    if (!isPause) {
        // std::cout << "gameview pause" << std::endl;
        isPause = true;
    }
}

void GameView::destory() {
    std::cout << "destory" << std::endl;
    delete console;
    console = nullptr;
}

void GameView::step() {
    // std::cout << "game thread start" << std::endl;
    auto lastTime = std::chrono::system_clock::now();
    while (mShow && !isPause) {
        readKeys();
        auto currentTime = std::chrono::system_clock::now();
        auto deltTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime);
        lastTime = currentTime;
        console->stepSeconds(deltTime.count() / 1000.0f);
    }
    // std::cout << "game thread end" << std::endl;
}

bool GameView::readKey(int key) {
    ImGuiIO &io = ImGui::GetIO();
    return io.KeysDown[key];
}

void GameView::readKeys() {
    console->setPressed(1, Button::A, readKey(GLFW_KEY_Z));
    console->setPressed(1, Button::B, readKey(GLFW_KEY_X));
    console->setPressed(1, Button::Select, readKey(GLFW_KEY_S));
    console->setPressed(1, Button::Start, readKey(GLFW_KEY_ENTER));
    console->setPressed(1, Button::Up, readKey(GLFW_KEY_UP));
    console->setPressed(1, Button::Down, readKey(GLFW_KEY_DOWN));
    console->setPressed(1, Button::Left, readKey(GLFW_KEY_LEFT));
    console->setPressed(1, Button::Right, readKey(GLFW_KEY_RIGHT));
}