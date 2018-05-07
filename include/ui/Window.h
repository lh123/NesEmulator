#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <mutex>

#include "ui/CreateServerView.h"
#include "ui/JoinServerView.h"
#include "ui/ID.h"

#include "nes/GameManager.h"
#include "net/GameProxy.h"

enum class GameType { Local, Host, Client };

class Window {
public:
    static constexpr int WIDTH = 256;
    static constexpr int HEIGHT = 240;
    static constexpr int SCALE = 2;
    static constexpr int PADDING = 0;

public:
    Window();
    ~Window();

    bool init(const char *title);
    void close();
    void run();

private:
    void initGUI();
    void renderGUI();
    void destoryGUI();

    void processGameFrame(Frame frame);
    void processGameKey(Button button, bool pressed);
    void renderGameFrame();

    void onClick(UI_ID id, void *data);

    void startGame(std::string path);
    void stopGame();

    void startGameHost(unsigned short port);
    void stopGameHost();

    void connectToHost(std::string ip, unsigned short port);
    void disconnect();

    bool readKey(int key);
    void readKeys();

private:
    GLFWwindow *mWindow;
    const char *mTitle;

    CreateServerView *mCreateServerView;
    JoinServerView *mJoinServerView;

    GameManager *mGameManager;
    GameProxy *mGameProxy;
    GameType mGameType;

    Frame mFrameBuffer;
    std::mutex mFrameBufferMutex;
    unsigned int mFrameTexture;
};

#endif