#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <string>
#include <cstdint>
#include <iostream>

class Serialize {
public:
    Serialize(std::iostream *stream) : mStream(stream) {}

    ~Serialize() { mStream->flush(); }

    template <class T>
    void write(const T &value) {
        mStream->write(reinterpret_cast<const char *>(&value), sizeof(T));
    }

    template <class T>
    Serialize &operator<<(const T &value) {
        write(value);
        return *this;
    }

    template <class T>
    void writeArray(const T *array, int size) {
        mStream->write(reinterpret_cast<const char *>(array), size);
    }

    template <class T>
    void read(T &value) {
        mStream->read(reinterpret_cast<char *>(&value), sizeof(T));
    }

    template <class T>
    Serialize &operator>>(T &value) {
        read(value);
        return *this;
    }

    template <class T>
    void readArray(T *array, int size) {
        mStream->read(reinterpret_cast<char *>(array), size);
    }

private:
    std::iostream *mStream;
};

#endif