#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "ui/UIComponent.h"
#include "nes/Console.h"

#include <string>
#include <thread>

class GameView : public UIComponent {
public:
    static constexpr float DEFAULT_WIDTH = 256;
    static constexpr float DEFAULT_HEIGHT = 240;

    GameView();
    virtual ~GameView();

    void setGamePath(const char *path);
    virtual void render() override;
    virtual void show() override;

private:
    void step();
    void start(const char *path);
    void resume();
    void pause();
    void destory();

    bool readKey(int key);
    void readKeys();

private:
    std::string nesName;
    bool isPause;
    bool isCollapsed;
    bool isFocused;
    unsigned int gameTexture;

    Console *console;
    std::thread *gameThread;
};

#endif