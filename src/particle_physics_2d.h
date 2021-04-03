#pragma once

#include "math_lib.h"

struct ParticlePhysics2D
{
    vec2 position[64];
    vec2 velocity[64];
    vec2 acceleration[64];
    u32 count;
};

void Integrate2D(
    vec2 *positions, vec2 *velocities, vec2 *accelerations, u32 count, f32 dt);

inline f32 Sat(f32 p0, f32 r0, f32 p1, f32 r1)
{
    f32 a = p0 - r0;
    f32 b = p0 + r0;
    f32 c = p1 - r1;
    f32 d = p1 + r1;

    f32 pen = 0.0f;
    if (a < c)
    {
        if (b < c)
        {
            // ------- a == b --- c === d
        }
        else
        {
            pen = b - c;
            // or ------- a ==== c #### b === d
        }
    }
    else
    {
        if (d < a)
        {
            // ----------- c ===== d --- a ===== b
        }
        else
        {
            pen = d - a;
            // ----------- c === a ## d ==== b
        }
    }

    return pen;
}
