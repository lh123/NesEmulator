#include "nes/Serialize.hpp"

Serialize::Serialize() {
    mWrite = nullptr;
    mRead = nullptr;

    mStart = nullptr;
    mEnd = nullptr;
}

Serialize::Serialize(const Serialize &other) : Serialize() {
    checkSize(other.size());
    memcpy(mWrite, other.mRead, other.size());
    mWrite += other.size();
}

Serialize::Serialize(Serialize &&other) {
    mWrite = other.mWrite;
    mRead = other.mRead;
    mStart = other.mStart;
    mEnd = other.mEnd;

    other.mWrite = nullptr;
    other.mRead = nullptr;
    other.mStart = nullptr;
    other.mEnd = nullptr;
}

Serialize::~Serialize() {
    if (mStart != nullptr) {
        operator delete(mStart);
    }
}

Serialize &Serialize::operator=(const Serialize &other) {
    if (other.size() > capacity()) {
        size_t newSize = (((other.size() - 1) / 2) + 1) * 2;
        char *newStart = reinterpret_cast<char *>(operator new(newSize));
        char *newEnd = newStart + newSize;
        char *newWrite = newStart;
        char *newRead = newStart;

        if (mStart != nullptr) {
            operator delete(mStart);
        }
        mWrite = newWrite;
        mRead = newRead;
        mStart = newStart;
        mEnd = newEnd;
    } else {
        mRead = mStart;
        mWrite = mStart;
    }
    memcpy(mWrite, other.mRead, other.size());
    mWrite += other.size();
    return *this;
}

Serialize &Serialize::operator=(Serialize &&other) {
    if (mStart != nullptr) {
        operator delete(mStart);
    }
    mWrite = other.mWrite;
    mRead = other.mRead;
    mStart = other.mStart;
    mEnd = other.mEnd;

    other.mWrite = nullptr;
    other.mRead = nullptr;
    other.mStart = nullptr;
    other.mEnd = nullptr;
    return *this;
}

void Serialize::writeToStream(std::ostream &stream) { stream.write(mRead, size()); }

void Serialize::readFromStream(std::istream &stream) {
    stream.seekg(0, std::ios::end);
    size_t fileSize = stream.tellg();
    stream.seekg(0, std::ios::beg);
    checkSize(fileSize);
    stream.read(mWrite, fileSize);
    mWrite += fileSize;
}

void Serialize::readFromMemory(const char *buffer, size_t size) {
    checkSize(size);
    memcpy(mWrite, buffer, size);
    mWrite += size;
}

bool Serialize::writeToMemory(char *buffer, size_t size) {
    if (size < this->size()) {
        return false;
    }
    memcpy(buffer, mRead, this->size());
    mRead += size;
    return true;
}

std::pair<const char *, size_t> Serialize::getData() const { return std::make_pair(mRead, size()); }

size_t Serialize::size() const { return static_cast<size_t>(mWrite - mRead); }

size_t Serialize::totalSize() const { return static_cast<size_t>(mWrite - mStart); }

size_t Serialize::capacity() const { return static_cast<size_t>(mEnd - mStart); }

void Serialize::clear() {
    mWrite = mStart;
    mRead = mStart;
}

void Serialize::checkSize(size_t writeSize) {
    if (mWrite + writeSize > mEnd) {
        size_t oldSize = size();
        size_t newSize = oldSize;

        if (oldSize == 0) {
            newSize = DEFAULT_SIZE;
        }

        while (newSize < oldSize + writeSize) {
            newSize *= 2;
        }

        std::cout << "Serialize resize " << oldSize << "->" << newSize << std::endl;

        char *newStart = reinterpret_cast<char *>(operator new(newSize));
        char *newEnd = newStart + newSize;
        char *newWrite = newStart;
        char *newRead = newStart;

        memcpy(newWrite, mRead, oldSize);
        newWrite += oldSize;

        if (mStart != nullptr) {
            operator delete(mStart);
        }
        mWrite = newWrite;
        mRead = newRead;
        mStart = newStart;
        mEnd = newEnd;
    }
}