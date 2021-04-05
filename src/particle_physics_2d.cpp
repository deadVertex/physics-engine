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

void UpdateAcceleration(
    vec2 *accelerations, vec2 *forces, f32 *invMasses, u32 count)
{
    for (u32 i = 0; i < count; ++i)
    {
        accelerations[i] = accelerations[i] + forces[i] * invMasses[i];
    }
}

u32 DetectCollisions(
    CollisionWorld *collisionWorld, OverlapEvent *events, u32 maxEvents)
{
    u32 count = 0;

    // Super dumb collision detection
    for (u32 circleIndex = 0; circleIndex < collisionWorld->circleCount;
         ++circleIndex)
    {
        vec2 p = collisionWorld->circles[circleIndex].center;
        f32 r = collisionWorld->circles[circleIndex].radius;
        for (u32 boxIndex = 0; boxIndex < collisionWorld->boxCount; ++boxIndex)
        {
            vec2 c = collisionWorld->boxes[boxIndex].center;
            vec2 h = collisionWorld->boxes[boxIndex].halfDims;

            f32 x = Sat(p.x, r, c.x, h.x);
            f32 y = Sat(p.y, r, c.y, h.y);
            if (x > 0.0f && y > 0.0f)
            {
                OverlapEvent *event = events + count++;
                event->circleIndex = circleIndex;
                // TODO: Use axis of particle velocity
                if (x < y)
                {
                    event->normal = Vec2(1, 0);
                }
                else
                {
                    event->normal = Vec2(0, 1);
                }
            }
        }
    }

    return count;
}

inline b32 GetBodyIndex(
    KinematicsEngine *kinematicsEngine, u32 circleIndex, u32 *index)
{
    for (u32 bindingIndex = 0;
         bindingIndex < kinematicsEngine->circleBindingCount; ++bindingIndex)
    {
        CollisionShapeBinding binding =
            kinematicsEngine->circleBindings[bindingIndex];
        if (binding.shapeIndex == circleIndex)
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
    // Sync collision world circles with kinematics engine
    for (u32 bindingIndex = 0;
         bindingIndex < kinematicsEngine->circleBindingCount; ++bindingIndex)
    {
        CollisionShapeBinding binding =
            kinematicsEngine->circleBindings[bindingIndex];
        collisionWorld->circles[binding.shapeIndex].center =
            kinematicsEngine->position[binding.bodyIndex];
    }

    // Sync collision world boxes with kinematics engine
    for (u32 bindingIndex = 0;
         bindingIndex < kinematicsEngine->boxBindingCount; ++bindingIndex)
    {
        CollisionShapeBinding binding =
            kinematicsEngine->boxBindings[bindingIndex];
        collisionWorld->boxes[binding.shapeIndex].center =
            kinematicsEngine->position[binding.bodyIndex];
    }

    OverlapEvent events[16];
    u32 eventCount =
        DetectCollisions(collisionWorld, events, ArrayCount(events));
    for (u32 eventIndex = 0; eventIndex < eventCount; ++eventIndex)
    {
        OverlapEvent *event = events + eventIndex;
        u32 bodyIndex = 0;
        if (GetBodyIndex(kinematicsEngine, event->circleIndex, &bodyIndex))
        {
            if (event->normal.x > 0.0f)
            {
                kinematicsEngine->velocity[bodyIndex].x =
                    -kinematicsEngine->velocity[bodyIndex].x;
            }
            else if (event->normal.y > 0.0f)
            {
                kinematicsEngine->velocity[bodyIndex].y =
                    -kinematicsEngine->velocity[bodyIndex].y;
            }
        }
    }
}
