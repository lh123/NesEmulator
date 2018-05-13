#include "ui/Window.h"
#include <iostream>

int main(int argc, char *argv[]) {
    Window window;
    if (!window.init("NES")) {
        std::cout << "window init error" << std::endl;
        return 0;
    }
    window.run();
    window.close();
    return 0;
}