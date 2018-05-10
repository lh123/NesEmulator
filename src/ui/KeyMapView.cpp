#include "ui/KeyMapView.h"
#include <iostream>
#include <sstream>
#include <string.h>
#include <GLFW/glfw3.h>

#include "ui/KeyConfig.h"

KeyMapView::KeyMapView(Config *config)
    : UIComponent("KeyMap"), mConfig(config), mKeyInputBuffer{0}, mCurrentBtn(Button::Up) {
    setWindowFlags(ImGuiWindowFlags_NoResize);
    for (int &key : mKeyCode) {
        key = -1;
    }
}

KeyMapView::~KeyMapView() { mConfig->save(); }

void KeyMapView::setOnClickListener(Clicklistenter listener) { mClickListener = listener; }

void KeyMapView::setKeyCode(int *keyCode) { memcpy(mKeyCode, keyCode, sizeof(int) * BUTTON_NUMBER); }

void KeyMapView::onShow() {
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        int keyCode = mKeyCode[i];
        getKeyNameByCode(keyCode, mKeyInputBuffer[i], BUFFER_SIZE);
    }
}

void KeyMapView::onBeforeRender() { ImGui::SetNextWindowSize(ImVec2(360, 150)); }

void KeyMapView::onRender() {

    ImGui::Columns(2);
    renderKeyButton(Button::Up);
    renderKeyButton(Button::Down);
    renderKeyButton(Button::Left);
    renderKeyButton(Button::Right);

    ImGui::NextColumn();
    renderKeyButton(Button::Select);
    renderKeyButton(Button::Start);
    renderKeyButton(Button::A);
    renderKeyButton(Button::B);

    ImGui::Columns(1);
    ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() - 120);
    ImGui::BeginChild("##Button", ImVec2(120, ImGui::GetItemsLineHeightWithSpacing()));
    if (ImGui::Button("Ok", ImVec2(50, -1))) {
        saveKeyConfig();        
        if (mClickListener != nullptr) {
            mClickListener(UI_ID::KeyMapView_Btn_Ok, nullptr);
        }
        close();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(70, -1))) {
        if (mClickListener != nullptr) {
            mClickListener(UI_ID::KeyMapView_Btn_Cancel, nullptr);
        }
        close();
    }
    ImGui::EndChild();

    ImGui::SetNextWindowSize(ImVec2(150, 60));
    ImGui::SetNextWindowPosCenter();
    if (ImGui::BeginPopup("KeyBind")) {
        ImGui::Text("Please press key");

        int keyCode = getPressedKeyCode();
        if (keyCode != -1) {
            getKeyNameByCode(keyCode, mKeyInputBuffer[(int)mCurrentBtn], BUFFER_SIZE);
            mKeyCode[(int)mCurrentBtn] = keyCode;
        }
        ImGui::Text(mKeyInputBuffer[(int)mCurrentBtn], ImVec4(0, 0, 0, 1));
        ImGui::EndPopup();
    }
}

void KeyMapView::renderKeyButton(Button btn) {
    ImGui::PushItemWidth(100);
    ImGui::InputText(getButtonName(btn), mKeyInputBuffer[(int)btn], BUFFER_SIZE, ImGuiInputTextFlags_ReadOnly);
    if (ImGui::IsItemClicked()) {
        ImGui::OpenPopup("KeyBind");
        mCurrentBtn = btn;
    }
    ImGui::PopItemWidth();
}

int KeyMapView::getPressedKeyCode() {
    ImGuiIO &io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.KeysDown); i++) {
        if (io.KeysDown[i]) {
            return i;
        }
    }
    return -1;
}

void KeyMapView::saveKeyConfig() {
    for (int i = 0; i < BUTTON_NUMBER; i++) {
        Button btn = (Button)i;
        if (mKeyCode >= 0) {
            mConfig->write("GameKey", getButtonName(btn), mKeyCode[i]);
        }
    }
    mConfig->save();
}