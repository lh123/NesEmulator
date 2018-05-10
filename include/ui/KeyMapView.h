#ifndef KEY_MAP_VIEW_H
#define KEY_MAP_VIEW_H
#include "ui/UIComponent.h"
#include "ui/Config.h"
#include "nes/Controller.h"
#include "ui/ID.h"

#include <functional>

class KeyMapView : public UIComponent {
public:
    static constexpr int BUFFER_SIZE = 16;
    static constexpr int BUTTON_NUMBER = 8;
    using Clicklistenter = std::function<void(UI_ID, void *)>;

    KeyMapView(Config *config);
    virtual ~KeyMapView();

    void setOnClickListener(Clicklistenter listener);
    void setKeyCode(int *keyCode);

protected:
    virtual void onShow() override;
    virtual void onBeforeRender() override;
    virtual void onRender() override;

private:
    int getPressedKeyCode();

    void renderKeyButton(Button btn);

    void saveKeyConfig();

private:
    Config *mConfig;
    char mKeyInputBuffer[BUTTON_NUMBER][BUFFER_SIZE];
    int mKeyCode[BUTTON_NUMBER];
    Button mCurrentBtn;

    Clicklistenter mClickListener;
};

#endif