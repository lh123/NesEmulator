#ifndef EVENT_H
#define EVENT_H

#include "nes/Controller.h"

class Serialize;

struct KeyEvent {
    int player;
    Button button;
    bool pressed;
};

struct SaveStateEvent {
    using SaveStateCallBack = void (*)(void *userData, const Serialize &);
    void *userData;
    SaveStateCallBack callback;
};

struct LoadStateEvent {
    Serialize *state;
};

struct GameRunStateEvent {
    bool pause;
    bool running;
    bool reset;
};

class Event {
public:
    enum class Type { Null, KeyEvent, SaveStateEvent, LoadStateEvent, GameRunStateEvent };

    Event();
    Event(const KeyEvent &event);
    Event(const SaveStateEvent &event);
    Event(const LoadStateEvent &event);
    Event(const GameRunStateEvent &event);

    Event &operator=(const Event &other);

    explicit operator KeyEvent &();
    explicit operator SaveStateEvent &();
    explicit operator LoadStateEvent &();
    explicit operator GameRunStateEvent &();

    const Type &getType() const;

    bool operator==(std::nullptr_t null) const;
    bool operator!=(std::nullptr_t null) const;
    bool isNull() const;

private:
    union Data {
        KeyEvent key;
        SaveStateEvent save;
        LoadStateEvent load;
        GameRunStateEvent state;
    };

private:
    Type mType;
    Data mData;
};

#endif