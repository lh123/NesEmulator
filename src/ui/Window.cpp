#include "ui/Window.h"

#include <iostream>
#include <fstream>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw_gl3.h>

#include "nes/Controller.h"
#include "ui/ImGuiExt.h"
#include "ui/KeyConfig.h"

Window::Window() : mWindow(nullptr), mGameProxy(nullptr), mPlayOneKeyBuffer{false}, mPlayTwoKeyBuffer{false}, mOpenAudio(true) {
    mAudio = new Audio;
    mGameManager = new GameManager;
    mGameProxy = new GameNetProxy(mGameManager);
    mConfig = new Config;
    if (!mConfig->loadFromDisk("./nes.ini")) {
        mConfig->save();
    }
    readAllKeyConfig();
}

Window::~Window() {
    delete mAudio;
    delete mGameProxy;
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

    if (mGameProxy->currentMode() == GameProxyMode::Master) {
        mGameProxy->stopServer();
        mGameProxy->disconnect();
    } else if (mGameProxy->currentMode() == GameProxyMode::Slave) {
        mGameProxy->disconnect();
    }

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
        startGameHost(hostData->port, hostData->skipFrame);
    } else if (id == UI_ID::JoinServerView_Btn_Connect) {
        JoinServerView::Data *clientData = reinterpret_cast<JoinServerView::Data *>(data);
        connectToHost(clientData->ip, clientData->port);
    } else if (id == UI_ID::KeyMapView_Btn_Ok) {
        readAllKeyConfig();
    }
}

void Window::startGame(std::string path) {
    if (mGameManager->isStop()) {
        mGameManager->startGame(path);
        mAudio->setAudioBuffer(mGameManager->getAudioBuffer());
        mAudio->play();
    }
}

void Window::stopGame() {
    if (!mGameManager->isStop()) {
        mGameManager->stop();
        mAudio->pause();
    }
}

void Window::startGameHost(unsigned short port, int frameSkip) {
    if (mGameProxy->currentMode() == GameProxyMode::Local) {
        mGameProxy->startServer(port);
        mGameProxy->setFrameSkip(frameSkip);
    }
}

void Window::stopGameHost() {
    if (mGameProxy->currentMode() == GameProxyMode::Master) {
        mGameProxy->stopServer();
    }
}

void Window::connectToHost(std::string ip, unsigned short port) {
    if (mGameProxy->currentMode() == GameProxyMode::Local) {
        mGameProxy->connectTo(ip, port);
    }
}

void Window::disconnect() {
    if (mGameProxy->currentMode() == GameProxyMode::Slave) {
        mGameProxy->disconnect();
    }
}

void Window::renderGUI() {

    ImGui_ImplGlfwGL3_NewFrame();
    char buffer[125] = {0};

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
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
            if (mGameProxy->currentMode() == GameProxyMode::Local) {
                if (ImGui::MenuItem("Create Server", nullptr, false, !mGameManager->isStop())) {
                    mCreateServerView->show();
                }
                if (ImGui::MenuItem("Connect to Server", nullptr, false, !mGameManager->isStop())) {
                    mJoinServerView->show();
                }
            } else if (mGameProxy->currentMode() == GameProxyMode::Master) {
                if (ImGui::MenuItem("Stop Server")) {
                    stopGameHost();
                }
            } else if (mGameProxy->currentMode() == GameProxyMode::Slave) {
                if (ImGui::MenuItem("Disconnect")) {
                    disconnect();
                }
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("Save", "Ctrl + S", false, !mGameManager->isStop())) {
                saveState();
            }
            if (ImGui::MenuItem("Load", "Ctrl + L", false, !mGameManager->isStop())) {
                loadState();
            }
            if (ImGui::MenuItem("Pause", nullptr, false, !mGameManager->isStop() && !mGameManager->isPause())) {
                mGameManager->pause();
                mGameProxy->pause();
            }
            if (ImGui::MenuItem("Resume", nullptr, false, !mGameManager->isStop() && mGameManager->isPause())) {
                mGameManager->resume();
                mGameProxy->resume();
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
        saveState();
    } else if (io.KeyCtrl && ImGui::IsKeyPressed(GLFW_KEY_L)) {
        loadState();
    }

    ImGui::Render();

    ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
}

void Window::processGameFrame(const Frame *frame) {
    std::unique_lock<std::mutex> tryLock(mFrameBufferMutex, std::defer_lock);
    if (tryLock.try_lock()) {
        mFrameBuffer = *frame;
    }
    mGameProxy->sync();
}

void Window::processGameKey(Button button, bool pressed) {
    int player = mGameProxy->currentMode() == GameProxyMode::Slave ? 2 : 1;
    bool *playBuffer = player == 1 ? mPlayOneKeyBuffer : mPlayTwoKeyBuffer;
    if (playBuffer[static_cast<int>(button)] != pressed) {
        playBuffer[static_cast<int>(button)] = pressed;
        mGameManager->setKeyPressed(player, button, pressed);
        mGameProxy->setKeyPressed(player, button, pressed);
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
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);

    if (!mGameManager->isStop() && !mGameManager->isPause()) {
        readKeys();
        std::lock_guard<std::mutex> lock(mFrameBufferMutex);
        glBindTexture(GL_TEXTURE_2D, mFrameTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Frame::WIDTH, Frame::HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, mFrameBuffer.pixel());
        ImGui::Image(reinterpret_cast<void *>(mFrameTexture), ImGui::GetContentRegionAvail());
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

void Window::saveState() {
    mGameManager->saveState([this](const Serialize &state) {
        mGameState = state;
        std::ofstream stateFile("state.sav", std::ios::out | std::ios::binary);
        if (stateFile.is_open()) {
            mGameState.writeToStream(stateFile);
        }
    });
}

void Window::loadState() {
    std::ifstream stateFile("state.sav", std::ios::in | std::ios::binary);
    if (stateFile.is_open()) {
        mGameState.clear();
        mGameState.readFromStream(stateFile);
        mGameManager->loadState(mGameState);
    }
}