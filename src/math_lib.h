#pragma once

#include <cfloat>
#include <cmath>

#include "platform.h"

#define PI 3.14159265359f
#define EPSILON FLT_EPSILON

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

inline vec2 operator-(vec2 a)
{
    vec2 v = {-a.x, -a.y};
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

inline f32 Sin(f32 x)
{
    f32 result = sinf(x);
    return result;
}

inline f32 Cos(f32 x)
{
    f32 result = cosf(x);
    return result;
}

inline f32 Tan(f32 x)
{
    f32 result = tanf(x);
    return result;
}

inline f32 Square(f32 x)
{
    f32 result = x * x;
    return result;
}

inline f32 Abs(f32 x)
{
    f32 result = abs(x);
    return result;
}
