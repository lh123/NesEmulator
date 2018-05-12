#ifndef JOIN_SERVER_VIEW_H
#define JOIN_SERVER_VIEW_H

#include "ui/ID.h"
#include "ui/UIComponent.h"
#include <functional>
#include <vector>

class JoinServerView : public UIComponent {
public:
    struct Data {
        std::string ip;
        unsigned short port;
    };

    using Clicklistenter = std::function<void(UI_ID, void *)>;
    static constexpr int PORT_INPUT_BUFFER_SIZE = 10;
    static constexpr int IP_INPUT_BUFFER_SIZE = 20;
    JoinServerView();
    virtual ~JoinServerView();
    void addClickListener(const Clicklistenter &listener);

protected:
    virtual void onShow() override;
    virtual void onBeforeRender() override;
    virtual void onRender() override;

private:
    char mIpInputBuffer[IP_INPUT_BUFFER_SIZE];
    char mPortInputBuffer[PORT_INPUT_BUFFER_SIZE];

    std::vector<Clicklistenter> mListeners;
};

#endif