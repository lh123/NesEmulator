#include "ui/Window.h"

#include <iostream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include "nes/Controller.h"
#include "ui/ImGuiExt.h"
#include "ui/KeyConfig.h"

Window::Window() : mWindow(nullptr), mGameProxy(nullptr), mGameType(GameType::Local), mOpenAudio(true) {
    mAudio = new Audio;
    mGameManager = new GameManager;
    mConfig = new Config;
    if (!mConfig->loadFromDisk("./nes.ini")) {
        mConfig->save();
    }
    readAllKeyConfig();
}

Window::~Window() {
    delete mAudio;
    delete mGameManager;
    delete mConfig;
}

bool Window::init(const char *title) {
    if (!glfwInit()) {
        return false;
    }

    mAudioInit = mAudio->init();
    if (mAudioInit) {
        mAudioInit = mAudio->openAudioDevice(44100);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mWindow = glfwCreateWindow(WIDTH * SCALE, HEIGHT * SCALE, title, nullptr, nullptr);
    if (mWindow == nullptr) {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(mWindow);

    if (!gladLoadGLLoader((GLADloadproc(glfwGetProcAddress)))) {
        return false;
    }

    glfwSwapInterval(1);
    return true;
}

void Window::run() {
    initGUI();

    while (!glfwWindowShouldClose(mWindow)) {
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        renderGUI();

        glfwSwapBuffers(mWindow);
        glfwPollEvents();
    }

    destoryGUI();
    glfwTerminate();
}

void Window::close() {}

void Window::initGUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui_ImplGlfwGL3_Init(mWindow, true);

    ImGui::StyleColorsDark();

    mCreateServerView = new CreateServerView();
    mJoinServerView = new JoinServerView();
    mKeyMapView = new KeyMapView(mConfig);

    auto clickListener = [this](UI_ID id, void *data) { onClick(id, data); };
    mCreateServerView->addClickListener(clickListener);
    mJoinServerView->addClickListener(clickListener);
    mKeyMapView->setOnClickListener(clickListener);

    glGenTextures(1, &mFrameTexture);
    glBindTexture(GL_TEXTURE_2D, mFrameTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    mGameManager->setOnFrameListener([this](const Frame *frame) { processGameFrame(frame); });
}

void Window::destoryGUI() {
    glDeleteTextures(1, &mFrameTexture);

    mGameManager->stop();

    mCreateServerView->close();
    mJoinServerView->close();
    mKeyMapView->close();

    delete mCreateServerView;
    delete mJoinServerView;
    delete mKeyMapView;

    ImGui_ImplGlfwGL3_Shutdown();
    ImGui::DestroyContext();
}

void Window::onClick(UI_ID id, void *data) {
    if (id == UI_ID::CreateServerView_Btn_Create) {
        CreateServerView::Data *hostData = reinterpret_cast<CreateServerView::Data *>(data);
        startGameHost(hostData->port, hostData->quality, hostData->skipFrame);
    } else if (id == UI_ID::JoinServerView_Btn_Connect) {
        JoinServerView::Data *clientData = reinterpret_cast<JoinServerView::Data *>(data);
        connectToHost(clientData->ip, clientData->port);
    } else if (id == UI_ID::KeyMapView_Btn_Ok) {
        readAllKeyConfig();
    }
}

void Window::startGame(std::string path) {
    if (mGameType == GameType::Host || mGameType == GameType::Local) {
        if (mGameManager->isStop()) {
            mGameManager->startGame(path);
            mAudio->setAudioBuffer(mGameManager->getAudioBuffer());
            mAudio->play();
        }
    }
}

void Window::stopGame() {
    if (mGameType == GameType::Host || mGameType == GameType::Local) {
        if (!mGameManager->isStop()) {
            mGameManager->stop();
            mAudio->pause();
        }
    }
}

void Window::startGameHost(unsigned short port, int quality, int frameSkip) {
    if (mGameType == GameType::Host) {
        return;
    }
    mGameType = GameType::Host;

    if (mGameProxy != nullptr) {
        return;
    }

    mGameProxy = new GameProxy(GameProxyMode::Host);
    mGameProxy->startServer(port);
    mGameProxy->setQuality(quality);
    mGameProxy->setFrameSkip(frameSkip);

    mGameProxy->setOnKeyListener([this](Button button, bool presses) {
        if (mGameManager->isStop() || mGameManager->isPause()) {
            return;
        }
        mGameManager->setKeyPressed(2, button, presses);
    });
    mAudio->setOnFillAudioListener([this](const float *audio, int length) {
        if (mGameManager->isStop() || mGameManager->isPause()) {
            return;
        }
        mGameProxy->sendAudioInfoToServer(audio, length);
    });
}

void Window::stopGameHost() {
    if (mGameType != GameType::Host) {
        return;
    }
    mGameType = GameType::Local;
    if (mGameProxy == nullptr) {
        return;
    }

    mGameProxy->stopServer();
    delete mGameProxy;
    mGameProxy = nullptr;

    mAudio->setOnFillAudioListener(nullptr);
}

void Window::connectToHost(std::string ip, unsigned short port) {
    if (mGameType == GameType::Client) {
        return;
    }

    mGameType = GameType::Client;

    if (mGameProxy != nullptr) {
        return;
    }

    mGameProxy = new GameProxy(GameProxyMode::Client);
    mGameProxy->connectTo(ip, port);
    mGameProxy->setOnFrameListener([this](const Frame *frame) { processGameFrame(frame); });
    mAudio->setAudioBuffer(mGameProxy->getAudioBuffer());
    mAudio->play();
}

void Window::disconnect() {
    if (mGameType != GameType::Client) {
        return;
    }

    mGameType = GameType::Local;

    if (mGameProxy == nullptr) {
        return;
    }

    mGameProxy->disconnect();
    delete mGameProxy;
    mGameProxy = nullptr;
}

void Window::renderGUI() {

    ImGui_ImplGlfwGL3_NewFrame();
    char buffer[125] = {0};

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", nullptr, false, mGameType != GameType::Client)) {
                if (ImGui::showFileDialog(buffer, 125)) {
                    startGame(buffer);
                }
            }

            if (ImGui::MenuItem("Stop", nullptr, false, !mGameManager->isStop())) {
                stopGame();
            }

            if (ImGui::MenuItem("Close")) {
                glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Multiplayer")) {
            if (mGameType == GameType::Local) {
                if (ImGui::MenuItem("Create Server")) {
                    mCreateServerView->show();
                }
                if (ImGui::MenuItem("Connect to Server", nullptr, false, mGameManager->isStop())) {
                    mJoinServerView->show();
                }
            } else if (mGameType == GameType::Host) {
                if (ImGui::MenuItem("Stop Server")) {
                    stopGameHost();
                }
            } else {
                if (ImGui::MenuItem("Disconnect")) {
                    disconnect();
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("Save", "Ctrl + S", false, !mGameManager->isStop())) {
                mGameManager->saveState();
            }
            if (ImGui::MenuItem("Load", "Ctrl + L", false, !mGameManager->isStop())) {
                mGameManager->loadState();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Setting")) {
            if (ImGui::MenuItem("KeyMap")) {
                mKeyMapView->setKeyCode(mKeyCode);
                mKeyMapView->show();
            }
            if (ImGui::MenuItem("Audio", nullptr, &mOpenAudio, mAudioInit)) {
                if (mOpenAudio) {
                    mAudio->play();
                } else {
                    mAudio->pause();
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    renderGameFrame();

    if (mCreateServerView->isShow()) {
        mCreateServerView->render();
    }
    if (mJoinServerView->isShow()) {
        mJoinServerView->render();
    }
    if (mKeyMapView->isShow()) {
        mKeyMapView->render();
    }

    ImGuiIO &io = ImGui::GetIO();
    if (io.KeyCtrl && ImGui::IsKeyPressed(GLFW_KEY_S)) {
        mGameManager->saveState();
    } else if (io.KeyCtrl && ImGui::IsKeyPressed(GLFW_KEY_L)) {
        mGameManager->loadState();
    }

    ImGui::Render();

    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::processGameFrame(const Frame *frame) {
    std::unique_lock<std::mutex> tryLock(mFrameBufferMutex, std::defer_lock);
    if (tryLock.try_lock()) {
        mFrameBuffer = *frame;
    }
}

void Window::processGameKey(Button button, bool pressed) {
    if (mGameProxy == nullptr) {
        // Local
        mGameManager->setKeyPressed(1, button, pressed);
    } else {
        if (mGameProxy->currentMode() == GameProxyMode::Host) {
            // Host
            mGameManager->setKeyPressed(1, button, pressed);
        } else {
            // Client
            mGameProxy->sendKeyInfoToServer(button, pressed);
        }
    }
}

void Window::renderGameFrame() {
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
    ImGui::SetNextWindowPos(ImVec2(0, 20));
    ImGui::SetNextWindowSize(ImVec2(WIDTH * SCALE, HEIGHT * SCALE - 20));
    ImGui::Begin("##GameView", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (!mGameManager->isStop() || (mGameProxy != nullptr && mGameProxy->currentMode() == GameProxyMode::Client)) {
        readKeys();
        std::lock_guard<std::mutex> lock(mFrameBufferMutex);
        glBindTexture(GL_TEXTURE_2D, mFrameTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Frame::WIDTH, Frame::HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE,
                     mFrameBuffer.pixel());
        ImGui::Image(reinterpret_cast<void *>(mFrameTexture), ImGui::GetContentRegionAvail());
        if (mGameProxy != nullptr && mGameProxy->currentMode() == GameProxyMode::Host) {
            mGameProxy->sendFrameInfoToServer(&mFrameBuffer);
        }
    }

    ImGui::End();
    ImGui::PopStyleVar(4);
}

bool Window::readKey(int key) {
    ImGuiIO &io = ImGui::GetIO();
    return io.KeysDown[key];
}

void Window::readKeys() {
    processGameKey(Button::A, readKey(mKeyCode[(int)Button::A]));
    processGameKey(Button::B, readKey(mKeyCode[(int)Button::B]));
    processGameKey(Button::Select, readKey(mKeyCode[(int)Button::Select]));
    processGameKey(Button::Start, readKey(mKeyCode[(int)Button::Start]));
    processGameKey(Button::Up, readKey(mKeyCode[(int)Button::Up]));
    processGameKey(Button::Down, readKey(mKeyCode[(int)Button::Down]));
    processGameKey(Button::Left, readKey(mKeyCode[(int)Button::Left]));
    processGameKey(Button::Right, readKey(mKeyCode[(int)Button::Right]));
}

void Window::readAllKeyConfig() {
    readKeyConfig(Button::A, &mKeyCode[0], GLFW_KEY_Z);
    readKeyConfig(Button::B, &mKeyCode[1], GLFW_KEY_X);
    readKeyConfig(Button::Select, &mKeyCode[2], GLFW_KEY_S);
    readKeyConfig(Button::Start, &mKeyCode[3], GLFW_KEY_ENTER);
    readKeyConfig(Button::Up, &mKeyCode[4], GLFW_KEY_UP);
    readKeyConfig(Button::Down, &mKeyCode[5], GLFW_KEY_DOWN);
    readKeyConfig(Button::Left, &mKeyCode[6], GLFW_KEY_LEFT);
    readKeyConfig(Button::Right, &mKeyCode[7], GLFW_KEY_RIGHT);
}

void Window::readKeyConfig(Button btn, int *keyCode, int defaultKeyCode) {
    bool success;
    int temp;
    temp = mConfig->readInt("GameKey", getButtonName(btn), &success);
    if (success) {
        *keyCode = temp;
    } else {
        *keyCode = defaultKeyCode;
    }
}