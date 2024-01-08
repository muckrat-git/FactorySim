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

template<typename x>
struct Rect {
    Vec2<x> position;
    Vec2<x> size;
};

float Modf(float a, float n) {
    return a - floor(a/n) * n;
}

float RotaryDistance(float a, float b) {
    return abs(Modf(a - b + PI, PI * 2) - PI);
}