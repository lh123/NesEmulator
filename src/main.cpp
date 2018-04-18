#include "nes/Console.h"
#include "ui/Window.h"
#include <cstdio>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::printf("please enter rom path\n");
        std::getchar();
        return 0;
    }
    Console col(argv[1]);
    if(!col.isOpenRom()) {
        std::getchar();
        return 0;
    }
    Window window{&col};
    if (!window.init("NES")) {
        std::printf("window init error\n");
        return 0;
    }
    window.run();
    window.close();
    return 0;
}