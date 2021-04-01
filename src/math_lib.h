#pragma once

#include "platform.h"

struct vec2
{
    f32 x;
    f32 y;
};

inline vec2 Vec2(f32 x, f32 y)
{
    vec2 v = {x, y};
    return v;
}

inline vec2 operator+(vec2 a, vec2 b)
{
    vec2 v = {a.x + b.x, a.y + b.y};
    return v;
}

inline vec2 operator-(vec2 a, vec2 b)
{
    vec2 v = {a.x - b.x, a.y - b.y};
    return v;
}

inline vec2 operator*(vec2 a, f32 b)
{
    vec2 v = {a.x * b, a.y * b};
    return v;
}
