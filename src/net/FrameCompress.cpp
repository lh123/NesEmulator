#include "net/FrameCompress.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <iostream>
#include <string.h>
#include <fstream>

void compressCallback(void *context, void *data, int size) {
    FrameCompress *compress = reinterpret_cast<FrameCompress *>(context);
    compress->onCompressCallback(data, size);
}

FrameCompress::FrameCompress() : mBufferSize(DEFAULT_BUFFER_SIZE), mWriteIndex(0) {
    mCompressBuffer = new char[DEFAULT_BUFFER_SIZE];
}

FrameCompress::~FrameCompress() { delete[] mCompressBuffer; }

std::pair<const char *, int> FrameCompress::compress(const Frame *frame, int quality) {
    mWriteIndex = 0;
    stbi_write_jpg_to_func(compressCallback, this, Frame::WIDTH, Frame::HEIGHT, 3, frame->pixel(), quality);
    int writeSize = mWriteIndex;
    return {mCompressBuffer, writeSize};
}

const Frame *FrameCompress::decompress(const char *data, int size) {
    const Frame *retData = nullptr;
    int width, height, channel;
    stbi_uc *image = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(data), size, &width, &height, &channel, 0);
    if (image != nullptr) {
        if (width == Frame::WIDTH && height == Frame::HEIGHT && channel == 3) {
            mDecompressBuffer.setData(image);
            retData = &mDecompressBuffer;
        }
        stbi_image_free(image);
    }
    return retData;
}

void FrameCompress::onCompressCallback(void *data, int size) {
    if (mWriteIndex + size > mBufferSize) {
        int newBufferSize = mBufferSize * 2;
        char *temp = new char[newBufferSize];
        memcpy(temp, mCompressBuffer, mWriteIndex);
        mBufferSize = newBufferSize;
        delete[] mCompressBuffer;
        mCompressBuffer = temp;
    }
    memcpy(mCompressBuffer + mWriteIndex, data, size);
    mWriteIndex += size;
}