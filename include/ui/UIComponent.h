#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H
#include <imgui/imgui.h>

class UIComponent {
public:
    UIComponent();
    ~UIComponent();

    virtual void render() = 0;

    bool isShow() const;

    virtual void show();
    virtual void close();

protected:
    bool mShow;
};

#endif