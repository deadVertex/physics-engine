#include "particle_physics_2d.h"

#include "collision_detection.h"

#include <cstdio>

void Integrate2D(
    vec2 *positions, vec2 *velocities, vec2 *accelerations, u32 count, f32 dt)
{
    for (u32 i = 0; i < count; ++i)
    {
        velocities[i] = velocities[i] + accelerations[i] * dt;
        positions[i] = positions[i] + velocities[i] * dt;
    }
}

void UpdateAcceleration(
    vec2 *accelerations, vec2 *forces, f32 *invMasses, u32 count)
{
    for (u32 i = 0; i < count; ++i)
    {
        accelerations[i] = accelerations[i] + forces[i] * invMasses[i];
    }
}

inline b32 GetBodyIndex(
    KinematicsEngine *kinematicsEngine, u32 shapeHandle, u32 *index)
{
    for (u32 bindingIndex = 0; bindingIndex < kinematicsEngine->bindingCount;
         ++bindingIndex)
    {
        CollisionShapeBinding binding =
            kinematicsEngine->collisionShapeBindings[bindingIndex];
        if (binding.shapeHandle == shapeHandle)
        {
            *index = binding.bodyIndex;
            return true;
        }
    }

    return false;
}

void ResolveCollisions(
    KinematicsEngine *kinematicsEngine, CollisionWorld *collisionWorld)
{
    // Sync collision world with kinematics engine
    for (u32 bindingIndex = 0; bindingIndex < kinematicsEngine->bindingCount;
         ++bindingIndex)
    {
        CollisionShapeBinding binding =
            kinematicsEngine->collisionShapeBindings[bindingIndex];

        u32 type = GetCollisionShapeHandleType(binding.shapeHandle);
        u32 idx = GetCollisionShapeHandleIndex(binding.shapeHandle);

        vec2 p = kinematicsEngine->position[binding.bodyIndex];

        switch (type)
        {
        case CollisionShape_Circle:
            collisionWorld->circles[idx].center = p;
            break;
        case CollisionShape_Box:
            collisionWorld->boxes[idx].center = p;
            break;
        default:
            break;
        }
    }

    Contact contacts[16];
    u32 count =
        GenerateContacts(contacts, ArrayCount(contacts), collisionWorld);

    // FIXME: This needs more work
    for (u32 contactIdx = 0; contactIdx < count; ++contactIdx)
    {
        Contact *contact = contacts + contactIdx;
        u32 bodyIndex = 0;
        if (GetBodyIndex(
                kinematicsEngine, contact->shapeHandles[0], &bodyIndex))
        {
            vec2 v = kinematicsEngine->velocity[bodyIndex];
            vec2 n = Dot(contact->normal, v) > 0.0f ? -contact->normal
                                                    : contact->normal;
            kinematicsEngine->velocity[bodyIndex] = v - Dot(v, n) * n * 2.0f;
            kinematicsEngine->position[bodyIndex] =
                kinematicsEngine->position[bodyIndex] + n * contact->pen;
        }

        if (GetBodyIndex(
                kinematicsEngine, contact->shapeHandles[1], &bodyIndex))
        {
            vec2 v = kinematicsEngine->velocity[bodyIndex];
            vec2 n = Dot(contact->normal, v) > 0.0f ? -contact->normal
                                                    : contact->normal;
            kinematicsEngine->velocity[bodyIndex] = v - Dot(v, n) * n * 2.0f;
            kinematicsEngine->position[bodyIndex] =
                kinematicsEngine->position[bodyIndex] + n * contact->pen;
        }
    }
}
