#ifndef FILTER_H
#define FILTER_H

#include <cstdint>

class Filter {
public:
    static constexpr float PI = 3.14159;

    enum class Type { LowPass, HighPass };

    Filter(Type type, uint32_t sampleRate, uint32_t freq);
    float step(float x);

private:
    float b0;
    float b1;
    float a1;

    float preY;
    float preX;

    Type type;
};

#endif