#pragma once

#include <iostream>
#include <raylib.h>
#include <raymath.h>

using namespace std;

// GLSL style vector definition shorthands
#define _vec22(x, y)  Vec2<float>{(float)x, (float)y}
#define _vec21(xy)    Vec2<float>{(float)xy, (float)xy}
#define _ivec22(x, y) Vec2<int>{(int)x, (int)y}
#define _ivec21(xy)   Vec2<int>{(int)xy, (int)xy}

// Provide macro overloading (https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments)
#define GET_VEC_MACRO(_1,_2,NAME,...) NAME
#define vec2(...)   GET_VEC_MACRO(__VA_ARGS__, _vec22, _vec21)(__VA_ARGS__)
#define ivec2(...)  GET_VEC_MACRO(__VA_ARGS__, _ivec22, _ivec21)(__VA_ARGS__)

template<typename type>
struct Vec2 {
    type x;
    type y;

    // Basic arithmatic operators
    Vec2 operator-(const Vec2 &rhs) const {
        return {
            x - rhs.x,
            y - rhs.y
        };
    }
    Vec2 operator+(const Vec2 &rhs) const {
        return {
            x + rhs.x,
            y + rhs.y
        };
    }
    Vec2 operator/(const Vec2 &rhs) const {
        return {
            x / rhs.x,
            y / rhs.y
        };
    }
    Vec2 operator/(const type &rhs) const {
        return {
            x / rhs,
            y / rhs
        };
    }
    Vec2 operator*(const Vec2 &rhs) const {
        return {
            x * rhs.x,
            y * rhs.y
        };
    }
    Vec2 operator*(const type &rhs) const {
        return {
            x * rhs,
            y * rhs
        };
    }

    Vec2 operator+=(const Vec2 &rhs) {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
    Vec2 operator-=(const Vec2 &rhs) {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    bool operator==(const Vec2 &rhs) const {
        return x == rhs.x && y == rhs.y;
    }

    bool operator!=(const Vec2 &rhs) const {
        return !operator==(rhs);
    }

    float Distance(Vec2 b) {
        return sqrtf(
            powf((float)x - (float)b.x, 2) + powf((float)y - (float)b.y, 2)
        );
    }

    string Str() {
        return to_string(x) + "," + to_string(y);
    }

    // Convert any Vec2 to a floating point Vec2
    Vec2<float> Float() {
        return Vec2<float>{(float)x, (float)y};
    }

    // Convert any Vec2 to an integer Vec2
    Vec2<int> Int() {
        return Vec2<int>{(int)x, (int)y};
    }

    // Rounds a Vec2 to the closest whole number
    Vec2<type> Round() {
        return Vec2<type>{round(x), round(y)};
    }

    // Provides a rounded integer Vec2
    Vec2<int> iRound() {
        return Round().Int();
    }

    // Provides a Vec2 rounded down to the nearest whole number
    Vec2<type> Floor() {
        return Vec2<type>{floor(x), floor(y)};
    }

    // Raylib Vector2 type cast
    operator Vector2() {
        return (Vector2){(float)x, (float)y};
    }
};