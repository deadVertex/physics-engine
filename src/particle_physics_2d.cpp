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

/*
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

    for (u32 a = 0; a < collisionWorld->boxCount; ++a)
    {
        Box boxA = collisionWorld->boxes[a];

        vec2 boxAVertices[4];
        GetBoxVertices(boxAVertices, ArrayCount(boxAVertices), boxA.center,
            boxA.halfDims, boxA.orientation);

        // Make everything relative to boxA.center
        for (u32 i = 0; i < 4; ++i)
        {
            boxAVertices[i] = boxAVertices[i] - boxA.center;
        }

        for (u32 b = 0; b < collisionWorld->boxCount; ++b)
        {
            if (a == b)
            {
                continue;
            }

            Box boxB = collisionWorld->boxes[b];

            vec2 boxBVertices[4];
            GetBoxVertices(boxBVertices, ArrayCount(boxBVertices), boxB.center,
                boxB.halfDims, boxB.orientation);

            vec2 axis[4];
            axis[0] = RotationMatrix(boxA.orientation)* Vec2(1, 0);
            axis[1] = RotationMatrix(boxA.orientation)* Vec2(0, 1);
            axis[2] = RotationMatrix(boxB.orientation)* Vec2(1, 0);
            axis[3] = RotationMatrix(boxB.orientation)* Vec2(0, 1);

            // Make everything relative to boxA.center
            for (u32 i = 0; i < 4; ++i)
            {
                boxBVertices[i] = boxBVertices[i] - boxA.center;
            }

            f32 minPen = F32_MAX;
            vec2 collisionNormal = {};
            for (u32 i = 0; i < 4; ++i)
            {
                SATIntervals intervals = CalculateIntervals(
                    boxA.center, axis[i], boxAVertices, 4, boxBVertices, 4);

                f32 pen = Sat2(intervals.values[0], intervals.values[1],
                        intervals.values[2], intervals.values[3]);
                printf("%g\n", pen);
                if (pen > 0.0f)
                {
                    if (pen < minPen)
                    {
                        minPen = pen;
                        collisionNormal = axis[i];
                    }
                }
                else
                {
                    minPen = F32_MAX;
                    break;
                }
            }

            if (minPen < F32_MAX)
            {
                printf("Collision detected: %g on axis (%g, %g) between boxes "
                       "%u and %u\n",
                    minPen, collisionNormal.x, collisionNormal.y, a, b);
            }
        }
    }

    return count;
}
*/

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
    for (u32 bindingIndex = 0;
         bindingIndex < kinematicsEngine->bindingCount; ++bindingIndex)
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

    /*
    OverlapEvent events[16];
    u32 eventCount =
        DetectCollisions(collisionWorld, events, ArrayCount(events));
    */
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
            kinematicsEngine->position[bodyIndex] =
                kinematicsEngine->position[bodyIndex] +
                contact->normal * contact->pen;

            if (contact->normal.x > 0.0f)
            {
                kinematicsEngine->velocity[bodyIndex].x =
                    -kinematicsEngine->velocity[bodyIndex].x;
            }
            else if (contact->normal.y > 0.0f)
            {
                kinematicsEngine->velocity[bodyIndex].y =
                    -kinematicsEngine->velocity[bodyIndex].y;
            }
        }

        if (GetBodyIndex(
                kinematicsEngine, contact->shapeHandles[1], &bodyIndex))
        {
            kinematicsEngine->position[bodyIndex] =
                kinematicsEngine->position[bodyIndex] +
                contact->normal * contact->pen;

            if (contact->normal.x > 0.0f)
            {
                kinematicsEngine->velocity[bodyIndex].x =
                    -kinematicsEngine->velocity[bodyIndex].x;
            }
            else if (contact->normal.y > 0.0f)
            {
                kinematicsEngine->velocity[bodyIndex].y =
                    -kinematicsEngine->velocity[bodyIndex].y;
            }
        }
    }
}
