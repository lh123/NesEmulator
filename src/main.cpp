#include "nes/Console.h"
#include "ui/Window.h"
#include <cstdio>

int main() {
    Console col{"E:\\VSCode\\NesEmulator\\rom\\hdl.nes"};
    Window window{&col};
    if (!window.init("NES")) {
        std::printf("window init error\n");
        return 0;
    }
    window.run();
    window.close();
    // col.cpu->write(0x6000, 0x80);
    // while(true) {
    //     col.cpu->step();
    //     if(col.cpu->read(0x6000) != 0x80) {
    //         uint16_t add = 0x6004;
    //         while(true) {
    //             char ch = char(col.cpu->read(add));
    //             std::printf("%c", ch);
    //             if(ch == '\0'){
    //                 break;
    //             }
    //             add++;
    //         }
    //         break;
    //     }
    // }
    return 0;
}