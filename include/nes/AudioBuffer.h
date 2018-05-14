#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include <cstdint>

class AudioBuffer {
public:
    AudioBuffer(int maxSize);
    ~AudioBuffer();

    void push(float data);
    float pop();
    int pop(float *buffer, int maxSize);
    void clear();
    bool isEmpty() const;
    int size() const;

private:
    float *mAudioBuffer;

    int mReadIndex;
    int mWriteIndex;

    int mDataCount;
    int mMaxSize;
};

#endif