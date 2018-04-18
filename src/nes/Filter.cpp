#include "nes/Filter.h"
#include <cmath>

Filter::Filter(Type type) : b0(0), b1(0), a1(0), preY(0), preX(0), type(type) {}

float Filter::step(float x) {
    float y = b0 * x + b1 * preX - a1 * preY;
    preY = y;
    preX = x;
    return y;
}

void Filter::setParamter(uint32_t sampleRate, uint32_t freq) {
    float c = sampleRate / PI / freq;
    float a1i = 1 / (1 + c);
    if(type == Type::HighPass) {
        b0 = c * a1i;
        b1 = -c * a1i;
    } else {
        b0 = a1i;
        b1 = a1i;
    }
    a1 = (1 - c) * a1i;
}