#include "nes/Console.h"
#include "ui/Window.h"
#include <cstdio>

int main() {
    Console col{"E:\\VSCode\\NesEmulator\\rom\\smb3.nes"};
    Window window{&col};
    if (!window.init("NES")) {
        std::printf("window init error\n");
        return 0;
    }
    window.run();
    window.close();
    return 0;
}