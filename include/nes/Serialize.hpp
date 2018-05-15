#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <string>
#include <cstdint>
#include <string.h>
#include <iostream>

class Serialize {
public:
    static constexpr int DEFAULT_SIZE = 13 * 1024;

    Serialize() {
        mSize = DEFAULT_SIZE;
        mBuffer = new char[mSize];
        mWriteSize = 0;
        mReadSize = 0;
    }

    Serialize(char *buffer, int size) {
        if (size <= DEFAULT_SIZE) {
            mSize = DEFAULT_SIZE;
        } else {
            mSize = size;
        }
        mBuffer = new char[mSize];
        memcpy(mBuffer, buffer, size);
        mSize = size;
        mWriteSize = size;
        mReadSize = 0;
    }

    Serialize(Serialize &&other) {
        mSize = other.mSize;
        mWriteSize = other.mWriteSize;
        mReadSize = other.mReadSize;
        mBuffer = other.mBuffer;
        other.mBuffer = nullptr;
    }

    ~Serialize() {
        if (mBuffer != nullptr) {
            delete[] mBuffer;
        }
    }

    template <class T>
    void write(const T &value) {
        int writeSize = sizeof(T);
        checkSize(writeSize);
        memcpy(mBuffer + mWriteSize, reinterpret_cast<const char *>(&value), writeSize);
        mWriteSize += writeSize;
    }

    template <class T>
    Serialize &operator<<(const T &value) {
        write(value);
        return *this;
    }

    template <class T>
    void writeArray(const T *array, int size) {
        checkSize(size);
        memcpy(mBuffer + mWriteSize, reinterpret_cast<const char *>(array), size);
        mWriteSize += size;
    }

    template <class T>
    void read(T &value) {
        int readSize = sizeof(T);
        memcpy(reinterpret_cast<char *>(&value), mBuffer + mReadSize, readSize);
        mReadSize += readSize;
    }

    template <class T>
    Serialize &operator>>(T &value) {
        read(value);
        return *this;
    }

    template <class T>
    void readArray(T *array, int size) {
        memcpy(reinterpret_cast<char *>(array), mBuffer + mReadSize, size);
        mReadSize += size;
    }

    void writeToStream(std::ostream &stream) { stream.write(mBuffer, mWriteSize); }

    void readFromStream(std::istream &stream) {
        stream.seekg(0, std::ios::end);
        int size = stream.tellg();
        stream.seekg(0, std::ios::beg);
        checkSize(size);
        stream.read(mBuffer, size);
    }

    std::pair<const char *, int> getData() const { return std::make_pair(mBuffer + mReadSize, mWriteSize - mReadSize); }

private:
    void checkSize(int writeSize) {
        if (mWriteSize + writeSize > mSize) {
            int newSize = mSize * 2;
            char *temp = new char[newSize];
            memcpy(temp, mBuffer, mWriteSize);
            delete[] mBuffer;
            mBuffer = temp;
            mSize = newSize;
        }
    }

private:
    char *mBuffer;
    int mWriteSize;
    int mReadSize;
    int mSize;
};

#endif