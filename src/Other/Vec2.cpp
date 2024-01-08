#pragma once

#include <iostream>
#include <raylib.h>
#include <raymath.h>

using namespace std;

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

    Vec2<float> Float() {
        return Vec2<float>{(float)x, (float)y};
    }

    operator Vector2() {
        return (Vector2){(float)x, (float)y};
    }
};

Vec2<float> fVec2FromStr(string s) {
    int count = 0;
    const char ** tokens = TextSplit(s.c_str(), ',', &count);
    if(count != 2) return {0};
    return Vec2<float>{
        stof(tokens[0]),
        stof(tokens[1])
    };
}

Vec2<double> dVec2FromStr(string s) {
    int count = 0;
    const char ** tokens = TextSplit(s.c_str(), ',', &count);
    if(count != 2) return {0};
    return Vec2<double>{
        stod(tokens[0]),
        stod(tokens[1])
    };
}

Vec2<long> lVec2FromStr(string s) {
    int count = 0;
    const char ** tokens = TextSplit(s.c_str(), ',', &count);
    if(count != 2) return {0};
    return Vec2<long>{
        stol(tokens[0]),
        stol(tokens[1])
    };
}

Vec2<int> iVec2FromStr(string s) {
    int count = 0;
    const char ** tokens = TextSplit(s.c_str(), ',', &count);
    if(count != 2) return {0};
    return Vec2<int>{
        stoi(tokens[0]),
        stoi(tokens[1])
    };
}