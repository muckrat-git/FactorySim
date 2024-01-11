#pragma once

#include "Vec2.cpp"
#include <stdint.h>

#define cstr const char *

#define modf0(x) (x - floorf(x))

#define u8 unsigned char
#define u16 unsigned short

template<typename a, typename b>
struct Pair {
    a first;
    b second;
};

// Vec2 based rectangle structure
template<typename x>
struct Rect {
    Vec2<x> position;
    Vec2<x> size;
};
// Vec2 based rectangle with absolute coords (not size)
template<typename x>
struct AbsRect {
    Vec2<x> a;
    Vec2<x> b;
};

float Modf(float a, float n) {
    return a - floor(a/n) * n;
}

float RotaryDistance(float a, float b) {
    return abs(Modf(a - b + PI, PI * 2) - PI);
}

float RotaryLerp(float a, float b, float x) {
    return atan2((1 - x) * sinf(a) + x * sinf(b), (1-x) * cosf(a) + x * cosf(b));
}

// Restricts the values available to 'count' amount
float Quantize(float value, float count, float min, float max) {
    float normal = Normalize(value, min, max);
    return roundf(normal * count) * max / count + min;
}