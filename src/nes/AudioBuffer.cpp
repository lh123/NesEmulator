#include "nes/AudioBuffer.h"
#include <string.h>

AudioBuffer::AudioBuffer(int maxSize) : mReadIndex(0), mWriteIndex(0), mDataCount(0), mMaxSize(maxSize) {
    mAudioBuffer = new float[mMaxSize]{0};
}

AudioBuffer::~AudioBuffer() { delete[] mAudioBuffer; }

void AudioBuffer::push(float data) {
    if (mDataCount < mMaxSize) {
        mAudioBuffer[mWriteIndex] = data;
        mWriteIndex++;
        if (mWriteIndex >= mMaxSize) {
            mWriteIndex = 0;
        }
        mDataCount++;
    }
}

int AudioBuffer::pop(float *buffer, int maxCount) {
    int popCount = maxCount > mDataCount ? mDataCount : maxCount;
    mDataCount -= popCount;

    if (mReadIndex + popCount < mMaxSize) {
        memcpy(buffer, mAudioBuffer + mReadIndex, popCount * sizeof(float));
        mReadIndex += popCount;
    } else {
        int firstPopCount = mMaxSize - mReadIndex;
        int lastPopCount = popCount - firstPopCount;
        memcpy(buffer, mAudioBuffer + mReadIndex, firstPopCount * sizeof(float));
        memcpy(buffer + firstPopCount, mAudioBuffer, lastPopCount * sizeof(float));
        mReadIndex = lastPopCount;
    }
    return popCount;
}

bool AudioBuffer::isEmpty() const { return mDataCount == 0; }

int AudioBuffer::size() const { return mDataCount; }
