#ifndef EVENT_H
#define EVENT_H

#include "nes/Controller.h"

struct KeyEvent {
    int player;
    Button button;
    bool pressed;
};

struct SaveStateEvent {
    char fileName[100];
};

struct LoadStateEvent {
    char fileName[100];
};

struct GameStateEvent {
    bool pause;
    bool running;
};

class Event {
public:
    enum class Type { KeyEvent, SaveStateEvent, LoadStateEvent, GameStateEvent };
    Event();
    Event(const KeyEvent &event);
    Event(const SaveStateEvent &event);
    Event(const LoadStateEvent &event);
    Event(const GameStateEvent &event);

    Event &operator=(const Event &other);

    operator KeyEvent();
    operator SaveStateEvent();
    operator LoadStateEvent();
    operator GameStateEvent();

    Type getType() const;

    bool operator==(std::nullptr_t null) const;
    bool operator!=(std::nullptr_t null) const;
    bool isNull() const;

private:
    union Data {
        KeyEvent key;
        SaveStateEvent save;
        LoadStateEvent load;
        GameStateEvent state;
    };

private:
    bool mIsNull;
    Type mType;
    Data mData;
};

#endif