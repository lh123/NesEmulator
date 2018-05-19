#ifndef CREATE_SERVER_VIEW_H
#define CREATE_SERVER_VIEW_H

#include "ui/ID.h"
#include "ui/UIComponent.h"
#include <functional>
#include <vector>

class CreateServerView : public UIComponent {
public:
    struct Data {
        unsigned short port;
        int skipFrame;
    };

    using Clicklistenter = std::function<void(UI_ID, void *)>;
    static constexpr int INPUT_BUFFER_SIZE = 10;
    CreateServerView();
    virtual ~CreateServerView();
    void addClickListener(const Clicklistenter &listener);

protected:
    virtual void onShow() override;
    virtual void onBeforeRender() override;
    virtual void onRender() override;

private:
    char mPortBuffer[INPUT_BUFFER_SIZE];
    char mSkipFrameBuffer[INPUT_BUFFER_SIZE];
    std::vector<Clicklistenter> mListeners;
};

#endif