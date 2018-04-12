#include "nes/Cartridge.h"
#include "nes/Console.h"
#include <cstdio>

int main() {
    Console col{"E:\\VSCode\\NesEmulator\\rom\\official_instructions.nes"};
    col.cpu->write(0x6000, 0x80);
    for (;;) {
        col.cpu->step();
        if(col.cpu->read(0x6000) != 0x80) {
            std::printf("status:%02X\n", col.cpu->read(0x6000));
            for(int i = 0;;i++){
                uint8_t c = col.cpu->read(0x6004 + i);
                if(c == 0){
                    break;
                }
                std::printf("%c", c);
            }
            break;
        }
    }
    return 0;
}