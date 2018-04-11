#ifndef IMAGE_H
#define IMAGE_H

#include <cstdint>

class Image {
public:
    struct RGBA {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        uint8_t a;
    };

public:
    Image(int width, int height);
    ~Image();
    
    void setRGBA(int x, int y, uint32_t rgba);

private:
    int width;
    int height;
    RGBA *data;
};

#endif