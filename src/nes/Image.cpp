#include "nes/Image.h"

Image::Image(int width, int height) : mWidth(width), mHeight(height) {
    data = new uint8_t[mWidth * mHeight * 4]{0};
}

Image::~Image() { delete[] data; }

void Image::setRGBA(int x, int y, RGBA rgba) {
    uint8_t r = (rgba >> 24) & 0xFF;
    uint8_t g = (rgba >> 16) & 0xFF;
    uint8_t b = (rgba >> 8) & 0xFF;
    uint8_t a = (rgba >> 0) & 0xFF;
    int index = (x + (y * mWidth)) * 4;
    data[index] = r;
    data[index + 1] = g;
    data[index + 2] = b;
    data[index + 3] = a;
}

int Image::width() const { return mWidth; }

int Image::height() const { return mHeight; }

uint8_t * Image::pixel() const { return data; }