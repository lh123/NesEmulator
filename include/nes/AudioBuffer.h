#ifndef AUDIO_BUFFER_H
#define AUDIO_BUFFER_H

#include <cstdint>

class AudioBuffer {
public:
    AudioBuffer(int size);
    ~AudioBuffer();

    void push(float data);
    float pop();
    bool isEmpty() const;
    int size() const;

private:
    float *buffer;

    int readIndex;
    int writeIndex;

    int dataCount;
    int maxSize;
};

#endif