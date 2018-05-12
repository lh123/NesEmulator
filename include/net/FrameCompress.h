#ifndef FRAME_COMPRESS_H
#define FRAME_COMPRESS_H

#include "nes/Frame.h"
#include <utility>

class FrameCompress {
public:
    static constexpr int DEFAULT_BUFFER_SIZE = 16 * 1024;
    FrameCompress();
    ~FrameCompress();

    std::pair<const char *, int> compress(const Frame *frame, int quality);
    const Frame *decompress(const char *data, int size);

    friend void compressCallback(void *context, void *data, int size);

private:
    void onCompressCallback(void *data, int size);

private:
    char *mCompressBuffer;
    Frame mDecompressBuffer;
    int mBufferSize;
    int mWriteIndex;
};

#endif