#pragma once

#include "math_lib.h"

struct Particle
{
    vec2 position;
    vec2 velocity;
    vec2 acceleration;
};

struct ParticlePhysics2D
{
    Particle particles[64];
    u32 particleCount;
};

extern void Integrate(ParticlePhysics2D *state, f32 dt);
