#ifndef MESSAGE_HEAD_H
#define MESSAGE_HEAD_H

struct PacketHead {
    static constexpr int TARGET_ALL = -1;
    // static constexpr const char MAGIC[10] = "NES_EMU_P";

    PacketHead();

    char magic[10];
    int origin;
    int target;

    int size;
    long long time;
};

#endif