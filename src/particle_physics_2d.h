#pragma once

#include "math_lib.h"

struct ParticlePhysics2D
{
    vec2 position[64];
    vec2 velocity[64];
    vec2 acceleration[64];
    u32 count;
};

extern void Integrate2D(
    vec2 *positions, vec2 *velocities, vec2 *accelerations, u32 count, f32 dt);
