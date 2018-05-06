#ifndef MESSAGE_HEAD_H
#define MESSAGE_HEAD_H

#include <cstdint>
#include <string.h>

enum class PacketType { ClientConnect, ClientDisconnect, Data };

struct PacketHead {
    static constexpr uint16_t MAGIC = 0xDC12;

    PacketHead() : magic(0), type(PacketType::Data) {}

    PacketHead(PacketType type) : magic(MAGIC), type(type) {}

    uint16_t magic;
    PacketType type;

    int size;
};

struct ClientInfoPacket {
    int clientId;
    char ip[20];
};

#endif