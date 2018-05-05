#include "ui/UIComponent.h"

UIComponent::UIComponent(const char *id) : mShowed(false), mFocused(false), mId(id), mFlags(0) {}

UIComponent::~UIComponent() {}

void UIComponent::setWindowFlags(ImGuiWindowFlags flags) { mFlags = flags; }

bool UIComponent::isShow() const { return mShowed; }

bool UIComponent::isFocused() const { return mFocused; }

void UIComponent::show() {
    if (!mShowed) {
        mShowed = true;
        onShow();
    }
}

void UIComponent::close() {
    if (mShowed) {
        mShowed = false;
        onClose();
    }
}

void UIComponent::render() {
    onBeforeRender();

    bool show = mShowed;

    if (ImGui::Begin(mId, &mShowed, mFlags)) {

        bool focused = ImGui::IsWindowFocused();
        if (mShowed) {
            if (mFocused && !focused) {
                onFocusedChanged(focused);
            } else if (!mFocused && focused) {
                onFocusedChanged(focused);
            }
        }
        mFocused = focused;

        onRender();
    }
    ImGui::End();
    onAfterRender();

    if (!mShowed && show) {
        onClose();
    }
}

void UIComponent::onShow() {}

void UIComponent::onClose() {}

void UIComponent::onFocusedChanged(bool focused) {}

void UIComponent::onBeforeRender() {}

void UIComponent::onAfterRender() {}