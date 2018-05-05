#include "nes/Image.h"
#include <string.h>

Image::Image() : mWidth(0), mHeight(0), data(nullptr) {}

Image::Image(int width, int height) : mWidth(width), mHeight(height) { data = new uint8_t[mWidth * mHeight * 4]{0}; }

Image::Image(const Image &other) {
    mWidth = other.mWidth;
    mHeight = other.mHeight;
    data = new uint8_t[mWidth * mHeight * 4];
    memcpy(data, other.data, mWidth * mHeight * 4 * sizeof(uint8_t));
}

Image::~Image() { delete[] data; }

Image &Image::operator=(const Image &other) {
    if (this != &other) {
        mWidth = other.mWidth;
        mHeight = other.mHeight;
        if (data != nullptr) {
            delete[] data;
            data = nullptr;
        }
        data = new uint8_t[mWidth * mHeight * 4];
        memcpy(data, other.data, mWidth * mHeight * 4 * sizeof(uint8_t));
    }
    return *this;
}

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

uint8_t *Image::pixel() const { return data; }