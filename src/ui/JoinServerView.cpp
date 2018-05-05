#include "ui/JoinServerView.h"
#include <cstring>

JoinServerView::JoinServerView() : UIComponent("CreateServer"), mIpInputBuffer{0}, mPortInputBuffer{0} {
    setWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
}

JoinServerView::~JoinServerView() {}

void JoinServerView::addClickListener(const Clicklistenter &listener) { mListeners.push_back(listener); }

void JoinServerView::onBeforeRender() { ImGui::SetNextWindowSize(ImVec2(270, 120)); }

void JoinServerView::onShow() {
    std::strcpy(mIpInputBuffer, "127.0.0.1");
    std::strcpy(mPortInputBuffer, "5000");
}

void JoinServerView::onRender() {
    ImGui::Text("Ip:");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##Ip", mIpInputBuffer, IP_INPUT_BUFFER_SIZE, ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();
    ImGui::Spacing();

    ImGui::Text("Port:");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##Port", mPortInputBuffer, PORT_INPUT_BUFFER_SIZE, ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();
    ImGui::Spacing();

    ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() - 110);
    ImGui::BeginChild("##Button", ImVec2(150, ImGui::GetItemsLineHeightWithSpacing()));
    if (ImGui::Button("Connect")) {
        Data data;
        data.ip = mIpInputBuffer;
        data.port = mPortInputBuffer;
        for (const Clicklistenter &l : mListeners) {
            l(UI_ID::JoinServerView_Btn_Connect, &data);
        }
        close();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        for (const Clicklistenter &l : mListeners) {
            l(UI_ID::JoinServerView_Btn_Cancel, nullptr);
        }
        close();
    }
    ImGui::EndChild();
}