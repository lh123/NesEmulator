#ifndef FILTER_H
#define FILTER_H

#include <cstdint>

class Filter {
public:
    static constexpr float PI = 3.14159;

    enum class Type { LowPass, HighPass };

    Filter(Type type);
    float step(float x);

    void setParamter(uint32_t sampleRate, uint32_t freq);

private:
    float b0;
    float b1;
    float a1;

    float preY;
    float preX;

    Type type;
};

#endif