#include "ui/Window.h"
#include <iostream>

#include "ui/Config.h"

int main(int argc, char *argv[]) {
    Window window;
    if (!window.init("NES")) {
        std::printf("window init error\n");
        return 0;
    }
    window.run();
    window.close();
    return 0;
}