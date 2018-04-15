#include "nes/Console.h"
#include "ui/Window.h"
#include <cstdio>

int main() {
    Console col{"E:\\VSCode\\NesEmulator\\rom\\apu\\apu_test.nes"};
    Window window{&col};
    if (!window.init("NES")) {
        std::printf("window init error\n");
        return 0;
    }
    window.run();
    window.close();
    return 0;
}