#ifndef GAME_VIEW_H
#define GAME_VIEW_H

#include "ui/UIComponent.h"
#include "nes/GameManager.h"
#include <string>
#include <thread>
#include <mutex>

class GameView : public UIComponent {
public:
    static constexpr float DEFAULT_WIDTH = 256;
    static constexpr float DEFAULT_HEIGHT = 240;

    GameView(GameManager *manager);
    
    virtual ~GameView();

protected:
    virtual void onShow() override;
    virtual void onClose() override;
    virtual void onFocusedChanged(bool focused) override;
    virtual void onBeforeRender() override;
    virtual void onRender() override;
    virtual void onAfterRender() override;

private:
    bool readKey(int key);
    void readKeys();

private:
    std::string nesName;
    unsigned int gameTexture;
    GameManager *gameManager;

    Image frameBuffer;

    std::mutex mFrameMutex;
};

#endif