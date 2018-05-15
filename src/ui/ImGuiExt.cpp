#include "ui/ImGuiExt.h"
#include <windows.h>

namespace ImGui {
bool showFileDialog(char *fileName, int size) {
    char currentPath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, currentPath);

    OPENFILENAME ofn = {0};

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = nullptr;
    ofn.lpstrFilter = TEXT("Nes File\0*.nes\0\0");
    ofn.nFilterIndex = 0;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = size;
    ofn.lpstrInitialDir = nullptr;
    ofn.lpstrTitle = TEXT("Please choose nes file");
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    bool ret = GetOpenFileName(&ofn);
    SetCurrentDirectory(currentPath);
    return ret;
}
}; // namespace ImGui