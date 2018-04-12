#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>

enum class Button { A, B, Select, Start, Up, Down, Left, Right };

class Controller {
public:
    Controller();
    ~Controller();

    void press(Button button);
    void release(Button button);

    void setPressed(Button button, bool pressed);

    uint8_t read();
    void write(uint8_t value);

private:
    uint8_t buttons[8];
    uint8_t index;
    uint8_t strobe;
};
#endif