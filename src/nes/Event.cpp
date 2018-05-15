#include "nes/Event.h"

Event::Event() : mIsNull(true) {}

Event::Event(const KeyEvent &event) : mIsNull(false) {
    mData.key = event;
    mType = Type::KeyEvent;
}

Event::Event(const SaveStateEvent &event) : mIsNull(false) {
    mData.save = event;
    mType = Type::SaveStateEvent;
}

Event::Event(const LoadStateEvent &event) : mIsNull(false) {
    mData.load = event;
    mType = Type::LoadStateEvent;
}

Event::Event(const GameStateEvent &event) : mIsNull(false) {
    mData.state = event;
    mType = Type::GameStateEvent;
}

Event::Type Event::getType() const { return mType; }

Event &Event::operator=(const Event &other) {
    mIsNull = other.mIsNull;
    mType = other.mType;
    mData = other.mData;
    return *this;
}

Event::operator KeyEvent() { return mData.key; }
Event::operator SaveStateEvent() { return mData.save; }
Event::operator LoadStateEvent() { return mData.load; }
Event::operator GameStateEvent() { return mData.state; }

bool Event::operator==(nullptr_t null) const { return mIsNull; }

bool Event::operator!=(nullptr_t null) const { return !mIsNull; }

bool Event::isNull() const { return mIsNull; }
