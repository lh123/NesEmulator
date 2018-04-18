#include "nes/Console.h"
#include "ui/Window.h"
#include <cstdio>

int main(int argc, char *argv[]) {
    Console col{"E:\\VSCode\\NesEmulator\\rom\\hdl.nes"};
    Window window{&col};
    if (!window.init("NES")) {
        std::printf("window init error\n");
        return 0;
    }
    window.run();
    window.close();
    return 0;
}