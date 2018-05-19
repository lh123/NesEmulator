#include "ui/CreateServerView.h"
#include <string.h>
#include <string>

CreateServerView::CreateServerView() : UIComponent("CreateServer"), mPortBuffer{0}, mSkipFrameBuffer{0} {
    setWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
}

CreateServerView::~CreateServerView() {}

void CreateServerView::addClickListener(const Clicklistenter &listener) { mListeners.push_back(listener); }

void CreateServerView::onBeforeRender() { ImGui::SetNextWindowSize(ImVec2(270, 110)); }

void CreateServerView::onShow() {
    strcpy(mPortBuffer, "5000");
    strcpy(mSkipFrameBuffer, "8");
}

void CreateServerView::onRender() {
    ImGui::Text("Port:");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##Port", mPortBuffer, INPUT_BUFFER_SIZE, ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();

    ImGui::Text("SkipFrame:");
    ImGui::SameLine();
    ImGui::PushItemWidth(44);
    ImGui::InputText("##SkipFrame", mSkipFrameBuffer, INPUT_BUFFER_SIZE, ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Spacing();

    ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() - 100);
    ImGui::BeginChild("##Button", ImVec2(150, ImGui::GetItemsLineHeightWithSpacing()));
    if (ImGui::Button("Create")) {
        Data data;
        data.port = std::stoi(mPortBuffer);
        data.skipFrame = std::stoi(mSkipFrameBuffer);

        for (const Clicklistenter &listener : mListeners) {
            listener(UI_ID::CreateServerView_Btn_Create, &data);
        }
        close();
    }
    ImGui::SameLine();
    if (ImGui::Button("Cancel")) {
        for (const Clicklistenter &l : mListeners) {
            l(UI_ID::CreateServerView_Btn_Cancel, nullptr);
        }
        close();
    }
    ImGui::EndChild();
}