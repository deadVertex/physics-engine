#include "particle_physics_2d.h"

void Integrate(ParticlePhysics2D *physics, f32 dt)
{
    for (u32 i = 0; i < physics->particleCount; ++i)
    {
        Particle *particle = physics->particles + i;
        particle->velocity = particle->velocity + particle->acceleration * dt;
        particle->position = particle->position + particle->velocity * dt;
    }
}
