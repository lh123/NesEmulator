#include "nes/Frame.h"
#include <string.h>

Frame::Frame() { mData = new uint8_t[SIZE]{0}; }

Frame::Frame(const uint8_t *data) {
    mData = new uint8_t[SIZE];
    ::memcpy(mData, data, SIZE);
}

Frame::Frame(const Frame &other) {
    mData = new uint8_t[SIZE];
    ::memcpy(mData, other.mData, SIZE);
}

Frame::Frame(Frame &&other) {
    mData = other.mData;
    other.mData = nullptr;
}

Frame::~Frame() {
    if (mData != nullptr) {
        delete[] mData;
    }
}

Frame &Frame::operator=(const Frame &other) {
    if (this != &other) {
        ::memcpy(mData, other.mData, SIZE);
    }
    return *this;
}

Frame &Frame::operator=(Frame &&other) {
    if (this != &other) {
        delete[] mData;
        mData = other.mData;
        other.mData = nullptr;
    }
    return *this;
}

void Frame::setRGBA(int x, int y, RGBA rgba) {
    uint8_t r = (rgba >> 24) & 0xFF;
    uint8_t g = (rgba >> 16) & 0xFF;
    uint8_t b = (rgba >> 8) & 0xFF;
    uint8_t a = (rgba >> 0) & 0xFF;
    int index = (x + (y * WIDTH)) * 4;
    mData[index] = r;
    mData[index + 1] = g;
    mData[index + 2] = b;
    mData[index + 3] = a;
}

uint8_t *Frame::pixel() const { return mData; }