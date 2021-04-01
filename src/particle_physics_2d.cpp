#include "particle_physics_2d.h"

void Integrate2D(
    vec2 *positions, vec2 *velocities, vec2 *accelerations, u32 count, f32 dt)
{
    for (u32 i = 0; i < count; ++i)
    {
        velocities[i] = velocities[i] + accelerations[i] * dt;
        positions[i] = positions[i] + velocities[i] * dt;
    }
}
