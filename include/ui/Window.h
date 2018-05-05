#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "nes/GameManager.h"

#include "ui/Audio.h"
#include "ui/GameView.h"
#include "ui/CreateServerView.h"
#include "ui/JoinServerView.h"
#include "ui/ID.h"

#include "net/Server.h"
#include "net/Client.h"

class Window {
public:
    static constexpr int WIDTH = 256;
    static constexpr int HEIGHT = 240;
    static constexpr int SCALE = 3;
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
    void onClick(UI_ID id, void *data);

private:
    Console *console;
    GLFWwindow *window;
    const char *title;
    Audio *audio;

    GameView *gameView;
    CreateServerView *createServerView;
    JoinServerView *joinServerView;

    Server *server;
    Client *client;

    GameManager *gameManager;
};

#endif