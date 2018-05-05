#include "ui/CreateServerView.h"
#include <cstring>

CreateServerView::CreateServerView() : UIComponent("CreateServer"), inputBuffer{0} {
    setWindowFlags(ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
}

CreateServerView::~CreateServerView() {}

void CreateServerView::addClickListener(const Clicklistenter &listener) { mListeners.push_back(listener); }

void CreateServerView::onBeforeRender() { ImGui::SetNextWindowSize(ImVec2(270, 90)); }

void CreateServerView::onShow() { std::strcpy(inputBuffer, "5000"); }

void CreateServerView::onRender() {
    ImGui::Text("Port:");
    ImGui::SameLine();
    ImGui::PushItemWidth(-1);
    ImGui::InputText("##Port", inputBuffer, INPUT_BUFFER_SIZE, ImGuiInputTextFlags_CharsDecimal);
    ImGui::PopItemWidth();
    ImGui::Spacing();

    ImGui::SetCursorPosX(ImGui::GetContentRegionAvailWidth() - 100);
    ImGui::BeginChild("##Button", ImVec2(150, ImGui::GetItemsLineHeightWithSpacing()));
    if (ImGui::Button("Create")) {
        for (const Clicklistenter &l : mListeners) {
            l(UI_ID::CreateServerView_Btn_Create, inputBuffer);
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