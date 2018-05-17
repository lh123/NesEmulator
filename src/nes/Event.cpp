#include "nes/Event.h"

Event::Event() : mType(Type::Null) {}

Event::Event(const KeyEvent &event) {
    mData.key = event;
    mType = Type::KeyEvent;
}

Event::Event(const SaveStateEvent &event) {
    mData.save = event;
    mType = Type::SaveStateEvent;
}

Event::Event(const LoadStateEvent &event) {
    mData.load = event;
    mType = Type::LoadStateEvent;
}

Event::Event(const GameRunStateEvent &event) {
    mData.state = event;
    mType = Type::GameRunStateEvent;
}

const Event::Type &Event::getType() const { return mType; }

Event &Event::operator=(const Event &other) {
    mType = other.mType;
    mData = other.mData;
    return *this;
}

Event::operator KeyEvent &() { return mData.key; }
Event::operator SaveStateEvent &() { return mData.save; }
Event::operator LoadStateEvent &() { return mData.load; }
Event::operator GameRunStateEvent &() { return mData.state; }

bool Event::operator==(nullptr_t null) const { return mType == Type::Null; }

bool Event::operator!=(nullptr_t null) const { return mType != Type::Null; }

bool Event::isNull() const { return mType == Type::Null; }
