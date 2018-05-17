#ifndef SERIALIZE_H
#define SERIALIZE_H

#include <string>
#include <cstdint>
#include <string.h>
#include <iostream>

class Serialize {
public:
    static constexpr int DEFAULT_SIZE = 1024;

    Serialize();
    ~Serialize();

    template <class T>
    void write(const T &value);

    template <class T>
    void writeArray(const T *array, size_t count);

    template <class T>
    void read(T &value);

    template <class T>
    void readArray(T *array, size_t count);

    template <class T>
    Serialize &operator<<(const T &value);

    template <class T>
    Serialize &operator>>(T &value);

    void writeToStream(std::ostream &stream);

    void readFromStream(std::istream &stream);

    std::pair<const char *, size_t> getData() const;

    size_t size() const;
    size_t capacity() const;

private:
    void checkSize(size_t writeSize);

private:
    char *mWrite;
    char *mRead;

    char *mStart;
    char *mEnd;
};

template <class T>
void Serialize::write(const T &value) {
    checkSize(sizeof(T));
    memcpy(mWrite, &value, sizeof(T));
    mWrite += sizeof(T);
}

template <class T>
void Serialize::writeArray(const T *array, size_t count) {
    checkSize(count * sizeof(T));
    memcpy(mWrite, array, count * sizeof(T));
    mWrite += count * sizeof(T);
}

template <class T>
void Serialize::read(T &value) {
    memcpy(&value, mRead, sizeof(T));
    mRead += sizeof(T);
}

template <class T>
void Serialize::readArray(T *array, size_t count) {
    memcpy(array, mRead, count * sizeof(T));
    mRead += count * sizeof(T);
}

template <class T>
Serialize &Serialize::operator<<(const T &value) {
    write(value);
    return *this;
}

template <class T>
Serialize &Serialize::operator>>(T &value) {
    read(value);
    return *this;
}

#endif