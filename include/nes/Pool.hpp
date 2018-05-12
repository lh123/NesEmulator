#ifndef POOL_HPP
#define POOL_HPP
#include <mutex>
#include <list>

template <class T>
class Pool {
public:
    Pool(int maxSize) : mMaxSize(maxSize), mSize(0) {}

    ~Pool() {
        for (T *t : mPoolList) {
            delete t;
        }
        mPoolList.clear();
    }

    T *get() {
        std::lock_guard<std::mutex> lock(mMutex);
        T *temp = nullptr;
        if (mPoolList.size() > 0) {
            temp = mPoolList.front();
            mPoolList.pop_front();
            mSize--;
        } else {
            if (mSize < mMaxSize) {
                temp = new T;
                mSize++;
            }
        }
        return temp;
    }

    bool free(T *t) {
        std::lock_guard<std::mutex> lock(mMutex);
        if (mSize < mMaxSize) {
            mPoolList.push_back(t);
            return true;
        } else {
            return false;
        }
    }

private:
    std::list<T *> mPoolList;
    std::mutex mMutex;

    int mMaxSize;
    int mSize;
};

#endif