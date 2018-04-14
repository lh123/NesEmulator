#include "nes/Controller.h"

Controller::Controller() : buttons{0}, index(0), strobe(0) {}

Controller::~Controller() {}

void Controller::press(Button button) {
    buttons[int(button)] = 1;
}

void Controller::release(Button button) {
    buttons[int(button)] = 0;
}

void Controller::setPressed(Button button, bool pressed) {
    if(pressed) {
        press(button);
    } else {
        release(button);
    }
}

uint8_t Controller::read() {
    uint8_t value = 0;
    if(index < 8) {
        value = buttons[index];
    } else {
        value = 0;
    }
    index++;
    if((strobe & 0x1) == 0x1) {
        index = 0;
    }
    return value;
}

void Controller::write(uint8_t value) {
    strobe = value;
    if((strobe & 0x1) == 0x1) {
        index = 0;
    }
}