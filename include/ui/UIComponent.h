#ifndef UI_COMPONENT_H
#define UI_COMPONENT_H
#include <imgui.h>

class UIComponent {
public:
    UIComponent(const char *id);

    virtual ~UIComponent();

    void setWindowFlags(ImGuiWindowFlags flags);
    bool isShow() const;
    bool isFocused() const;

    void show();
    void close();
    void render();

protected:
    virtual void onShow();
    virtual void onClose();
    virtual void onFocusedChanged(bool focused);
    virtual void onBeforeRender();
    virtual void onRender() = 0;
    virtual void onAfterRender();

private:
    bool mShowed;
    bool mFocused;
    const char *mId;

    ImGuiWindowFlags mFlags;
};

#endif