#include "ui/KeyConfig.h"
#include <GLFW/glfw3.h>
#include <sstream>
#include <string.h>

static int defaultKeyMap[] = {GLFW_KEY_Z,  GLFW_KEY_X,    GLFW_KEY_S,    GLFW_KEY_ENTER,
                              GLFW_KEY_UP, GLFW_KEY_DOWN, GLFW_KEY_LEFT, GLFW_KEY_RIGHT};

static const char *keyNameMap[] = {"A", "B", "Select", "Start", "Up", "Down", "Left", "Right"};

int getDefaultKey(Button btn) { return defaultKeyMap[(int)btn % 8]; }

const char *getButtonName(Button btn) { return keyNameMap[(int)btn % 8]; }

void getKeyNameByCode(int code, char *buffer, int size) {
    std::stringstream keyStream;
    if (code == GLFW_KEY_SPACE) {
        keyStream << "SPACE";
    } else if (code == GLFW_KEY_ENTER) {
        keyStream << "ENTER";
    } else if (code >= GLFW_KEY_KP_0 && code <= GLFW_KEY_KP_9) {
        keyStream << "NUM" << code - GLFW_KEY_KP_0;
    } else if (code > GLFW_KEY_SPACE && code <= GLFW_KEY_GRAVE_ACCENT) {
        keyStream << (char)code;
    } else if (code >= GLFW_KEY_F1 && code <= GLFW_KEY_F12) {
        keyStream << "F" << code - GLFW_KEY_F1 + 1;
    } else {
        keyStream << "KeyCode:" << code;
    }
    std::string temp = keyStream.str();
    if (size < (int)temp.size() + 1) {
        return;
    } else {
        strcpy(buffer, temp.c_str());
    }
}