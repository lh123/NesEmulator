#include "nes/Serialize.hpp"

Serialize::Serialize() {
    mWrite = nullptr;
    mRead = nullptr;

    mStart = nullptr;
    mEnd = nullptr;
}

Serialize::~Serialize() {
    if (mStart != nullptr) {
        operator delete(mStart);
    }
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

std::pair<const char *, size_t> Serialize::getData() const { return std::make_pair(mRead, size()); }

size_t Serialize::size() const { return static_cast<size_t>(mWrite - mRead); }

size_t Serialize::capacity() const { return static_cast<size_t>(mEnd - mStart); }

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