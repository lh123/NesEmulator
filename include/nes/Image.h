#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>

class Image {
public:
    using RGBA = uint32_t;
    Image(int width, int height);
    ~Image();

    void setRGBA(int x, int y, RGBA rgba);
    int width() const;
    int height() const;
    uint8_t *pixel() const;

private:
    int mWidth;
    int mHeight;
    uint8_t *data;
};

#endif