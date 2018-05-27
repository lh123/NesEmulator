#ifndef GAME_PACKET_H
#define GAME_PACKET_H

#include "nes/Frame.h"
#include "nes/Controller.h"

enum class GamePacketType { Pause, Resume, Key, Sync };

struct GamePacketHead {
    GamePacketType type;
    size_t size;
};

struct GamePausePacket {};

struct GameResumePacket {};

struct GameKeyPacket {
    int player;
    Button button;
    bool pressed;
};

// struct SyncPacket {
//     char *syncData;
// };

uint16_t checkSum(const char *data, size_t len);

#endif