#ifndef FRAME_H
#define FRAME_H

#include <cstdint>

class Frame {
public:
    static constexpr int WIDTH = 256;
    static constexpr int HEIGHT = 240;
    static constexpr int SIZE = WIDTH * HEIGHT * 3;

    using RGBA = uint32_t;
    Frame();
    Frame(const uint8_t *data);
    Frame(const Frame &other);
    Frame(Frame &&other);

    ~Frame();

    Frame &operator=(const Frame &other);
    Frame &operator=(Frame &&other);

    void setData(const uint8_t *data);
    void setRGBA(int x, int y, RGBA rgba);
    uint8_t *pixel() const;

private:
    uint8_t *mData;
};

#endif