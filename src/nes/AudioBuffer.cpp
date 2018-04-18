#include "nes/AudioBuffer.h"

AudioBuffer::AudioBuffer(int size) : readIndex(0), writeIndex(0), dataCount(0), maxSize(size) {
    buffer = new float[size]{0};
}

AudioBuffer::~AudioBuffer() { delete[] buffer; }

void AudioBuffer::push(float data) {
    if (dataCount < maxSize) {
        buffer[writeIndex] = data;
        writeIndex++;
        if (writeIndex >= maxSize) {
            writeIndex = 0;
        }
        dataCount++;
    }
}

float AudioBuffer::pop() {
    if (dataCount > 0) {
        float data = buffer[readIndex];
        readIndex++;
        if (readIndex >= maxSize) {
            readIndex = 0;
        }
        dataCount--;
        return data;
    } else {
        return 0;
    }
}

bool AudioBuffer::isEmpty() { return dataCount == 0; }
