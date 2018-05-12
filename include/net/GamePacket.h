#ifndef GAME_PACKET_H
#define GAME_PACKET_H

#include "nes/Frame.h"
#include "nes/Controller.h"
#include "net/FrameCompress.h"

enum class GamePacketType { Frame, Key, Audio };

struct GamePacketHead {
    GamePacketType type;
    int size;
};

// struct GameFramePacket {
//     uint8_t imageData[Frame::SIZE];
// };

struct GameKeyPacket {
    Button button;
    bool pressed;
};

struct GameAudioPacket {};

#endif