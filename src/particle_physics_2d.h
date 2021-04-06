#pragma once

#include "math_lib.h"

#include "collision_detection.h"

struct CollisionShapeBinding
{
    u32 bodyIndex;
    u32 shapeHandle;
};

struct KinematicsEngine
{
    vec2 position[16];
    vec2 velocity[16];
    vec2 acceleration[16];
    vec2 forceAccumulation[64];
    f32 invMass[64];
    u32 count;

    CollisionShapeBinding collisionShapeBindings[16];
    u32 bindingCount;
};

void Integrate2D(
    vec2 *positions, vec2 *velocities, vec2 *accelerations, u32 count, f32 dt);

void UpdateAcceleration(
    vec2 *accelerations, vec2 *forces, f32 *invMasses, u32 count);

void ResolveCollisions(
    KinematicsEngine *kinematicsEngine, CollisionWorld *collisionWorld);
