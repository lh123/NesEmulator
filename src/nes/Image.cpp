#include "nes/Image.h"

Image::Image(int width, int height) : width(width), height(height) {
    data = new RGBA[width * height]{0};
}

Image::~Image() { delete[] data; }

void Image::setRGBA(int x, int y, uint32_t rgba) {
    uint8_t r = (rgba >> 24) & 0xFF;
    uint8_t g = (rgba >> 16) & 0xFF;
    uint8_t b = (rgba >> 8) & 0xFF;
    uint8_t a = (rgba >> 0) & 0xFF;
    data[x * y] = {r, g, b, a};
}