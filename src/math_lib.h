#pragma once

#include <cfloat>
#include <cmath>

#include "platform.h"

#define PI 3.14159265359f
#define EPSILON FLT_EPSILON
#define F32_MAX FLT_MAX

struct vec2
{
    f32 x;
    f32 y;
};

union mat2
{
    vec2 columns[2];
    f32 data[4];
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

inline f32 Dot(vec2 a, vec2 b)
{
    f32 result = a.x * b.x + a.y * b.y;
    return result;
}

inline mat2 Mat2(f32 a, f32 b, f32 c, f32 d)
{
    mat2 result = {a, b, c, d};
    return result;
}

inline vec2 operator*(mat2 a, vec2 b)
{
    vec2 result;
    result.x = a.data[0] * b.x + a.data[2] * b.y;
    result.y = a.data[1] * b.x + a.data[3] * b.y;
    return result;
}

inline vec2 Perpendicular(vec2 a)
{
    vec2 result = {a.y, -a.x};
    return result;
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

inline f32 Max(f32 a, f32 b)
{
    f32 result = a > b ? a : b;
    return result;
}

inline f32 Min(f32 a, f32 b)
{
    f32 result = a < b ? a : b;
    return result;
}

inline vec2 Max(vec2 a, vec2 b)
{
    vec2 result;
    result.x = Max(a.x, b.x);
    result.y = Max(a.y, b.y);
    return result;
}

inline vec2 Min(vec2 a, vec2 b)
{
    vec2 result;
    result.x = Min(a.x, b.x);
    result.y = Min(a.y, b.y);
    return result;
}

inline mat2 RotationMatrix(f32 angle)
{
    /*
     * | Cos(angle) -Sin(angle) |
     * | Sin(angle)  Cos(angle) |
     *
     */
    mat2 result;
    result.data[0] = Cos(angle);
    result.data[1] = Sin(angle);
    result.data[2] = -Sin(angle);
    result.data[3] = Cos(angle);

    return result;
}
