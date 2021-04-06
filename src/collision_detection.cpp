#include "collision_detection.h"

#include <cstdio>

CollisionWorld CreateCollisionWorld(
    Box *boxes, u32 boxCount, Circle *circles, u32 circleCount)
{
    CollisionWorld result = {};
    result.boxes = boxes;
    result.maxBoxes = boxCount;
    result.circles = circles;
    result.maxCircles = circleCount;

    return result;
}

u32 AddBox(CollisionWorld *collisionWorld, vec2 center, vec2 halfDims,
    f32 orientation)
{
    Assert(collisionWorld->boxCount < collisionWorld->maxBoxes);
    u32 idx = collisionWorld->boxCount++;
    Box *box = collisionWorld->boxes + idx;
    box->center = center;
    box->halfDims = halfDims;
    box->orientation = orientation;

    return BuildCollisionShapeHandle(CollisionShape_Box, idx);
}

u32 AddCircle(CollisionWorld *collisionWorld, vec2 center, f32 radius)
{
    Assert(collisionWorld->circleCount < collisionWorld->maxCircles);
    u32 idx = collisionWorld->circleCount++;
    Circle *circle = collisionWorld->circles + idx;
    circle->center = center;
    circle->radius = radius;

    return BuildCollisionShapeHandle(CollisionShape_Circle, idx);
}

static u32 BuildAabbs(Aabb *aabbs, u32 maxAabbs, u32 *shapeHandles,
    CollisionWorld *collisionWorld)
{
    u32 count = 0;
    for (u32 circleIndex = 0; circleIndex < collisionWorld->circleCount;
         ++circleIndex)
    {
        Circle *circle = collisionWorld->circles + circleIndex;
        Assert(count < maxAabbs);
        u32 idx = count++;
        aabbs[idx] = CalculateCircleAabb(circle->center, circle->radius);
        shapeHandles[idx] =
            BuildCollisionShapeHandle(CollisionShape_Circle, circleIndex);
    }

    for (u32 boxIndex = 0; boxIndex < collisionWorld->boxCount; ++boxIndex)
    {
        Box *box = collisionWorld->boxes + boxIndex;
        Assert(count < maxAabbs);
        u32 idx = count++;
        aabbs[idx] =
            CalculateBoxAabb(box->center, box->halfDims, box->orientation);
        shapeHandles[idx] =
            BuildCollisionShapeHandle(CollisionShape_Box, boxIndex);
    }

    return count;
}

// TODO: Consolidate with GenerateBoxVertices
// DUPLICATE: particle_physics_2d.h
inline u32 GetBoxVertices(vec2 *vertices, u32 maxVertices, vec2 center,
        vec2 halfDims, f32 orientation)
{
    Assert(maxVertices >= 4);

    mat2 rotation = RotationMatrix(orientation);
    vertices[0] = center + rotation * Vec2(-halfDims.x, -halfDims.y);
    vertices[1] = center + rotation * Vec2(halfDims.x, -halfDims.y);
    vertices[2] = center + rotation * Vec2(halfDims.x, halfDims.y);
    vertices[3] = center + rotation * Vec2(-halfDims.x, halfDims.y);

    return 4;
}

// DUPLICATE: particle_physics_2d.h
inline f32 Support(vec2 *vertices, u32 count, vec2 d)
{
    f32 max = -F32_MAX;
    for (u32 idx = 0; idx < count; ++idx)
    {
        f32 t = Dot(vertices[idx], d);
        max = Max(max, t);
    }

    return max;
}

// TODO: Better implementation of this
b32 BoxVsBoxNarrowPhase(Contact *contact, Box boxA, Box boxB)
{
    // Get list of vertices for each shape
    vec2 boxAVertices[4];
    GetBoxVertices(boxAVertices, ArrayCount(boxAVertices), boxA.center,
        boxA.halfDims, boxA.orientation);

    vec2 boxBVertices[4];
    GetBoxVertices(boxBVertices, ArrayCount(boxBVertices), boxB.center,
            boxB.halfDims, boxB.orientation);

    // Make all vertices relative to boxA.center
    for (u32 i = 0; i < 4; ++i)
    {
        boxAVertices[i] = boxAVertices[i] - boxA.center;
        boxBVertices[i] = boxBVertices[i] - boxA.center;
    }

    // Test every vertex on both shapes against axis for each edge (minus
    // duplicate edges)
    vec2 axis[4];
    axis[0] = RotationMatrix(boxA.orientation)* Vec2(1, 0);
    axis[1] = RotationMatrix(boxA.orientation)* Vec2(0, 1);
    axis[2] = RotationMatrix(boxB.orientation)* Vec2(1, 0);
    axis[3] = RotationMatrix(boxB.orientation)* Vec2(0, 1);

    f32 minPen = F32_MAX;
    vec2 collisionNormal = {};
    for (u32 i = 0; i < 4; ++i)
    {
        // Test each axis
        vec2 testAxis = axis[i];

        // Shape A
        f32 t0 = Support(boxAVertices, 4, testAxis);
        f32 t1 = -Support(boxAVertices, 4, -testAxis);

        // Shape B
        f32 t2 = Support(boxBVertices, 4, testAxis);
        f32 t3 = -Support(boxBVertices, 4, -testAxis);

        // Sort projection intervals
        f32 a0 = Min(t0, t1);
        f32 a1 = Max(t0, t1);
        f32 b0 = Min(t2, t3);
        f32 b1 = Max(t2, t3);

        printf("a0 = %g\n", a0);
        printf("a1 = %g\n", a1);
        printf("b0 = %g\n", b0);
        printf("b1 = %g\n", b1);

        f32 pen = Min(a1, b1) - Max(a0, b0);
        printf("pen = %g\n", pen);

        if (pen > 0.0f)
        {
            if (pen < minPen)
            {
                minPen = pen;
                collisionNormal = testAxis;
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
        printf("Collision detected: %g on axis (%g, %g)\n", minPen,
            collisionNormal.x, collisionNormal.y);

        contact->pen = minPen;
        contact->normal = collisionNormal;

        // TODO: How do we calculate contact point?

        return true;
    }

    return false;
}

u32 ProcessNarrowPhase(Contact *contacts, u32 maxContacts,
    NarrowPhaseEntry *narrowPhaseEntries, u32 narrowPhaseCount,
    CollisionWorld *collisionWorld)
{
    u32 count = 0;

    for (u32 narrowPhaseIndex = 0; narrowPhaseIndex < narrowPhaseCount;
         narrowPhaseIndex++)
    {
        NarrowPhaseEntry narrowPhase = narrowPhaseEntries[narrowPhaseIndex];
        u32 typeA = GetCollisionShapeHandleType(narrowPhase.handleA);
        u32 typeB = GetCollisionShapeHandleType(narrowPhase.handleB);

        u32 idxA = GetCollisionShapeHandleIndex(narrowPhase.handleA);
        u32 idxB = GetCollisionShapeHandleIndex(narrowPhase.handleB);

        if (typeA == CollisionShape_Circle && typeB == CollisionShape_Circle)
        {
            // Circle vs Circle
            // TODO: Implement this
        }
        else if (typeA == CollisionShape_Box && typeB == CollisionShape_Box)
        {
            // Box vs Box
            Box boxA = collisionWorld->boxes[idxA];
            Box boxB = collisionWorld->boxes[idxB];

            Contact newContact = {};
            if (BoxVsBoxNarrowPhase(&newContact, boxA, boxB))
            {
                newContact.shapeHandles[0] = narrowPhase.handleA;
                newContact.shapeHandles[1] = narrowPhase.handleB;
                contacts[count++] = newContact;
            }
        }
        else
        {
            // Circle vs Box or Box vs Circle
            // TODO: Implement this
        }
    }

    return count;
}

u32 GenerateContacts(
    Contact *contacts, u32 maxContacts, CollisionWorld *collisionWorld)
{
    // Perform broadphase intersection tests

    // Generate list of Aabbs for every shape in the collision world
    Aabb aabbs[64];
    u32 shapeHandles[64];
    u32 broadphaseCount =
        BuildAabbs(aabbs, ArrayCount(aabbs), shapeHandles, collisionWorld);

    NarrowPhaseEntry narrowPhaseEntries[64];
    u32 narrowPhaseCount = 0;

    // Test each against each other O(n^2) style
    for (u32 idxA = 0; idxA < broadphaseCount; ++idxA)
    {
        for (u32 idxB = idxA + 1; idxB < broadphaseCount; ++idxB)
        {
            Aabb a = aabbs[idxA];
            Aabb b = aabbs[idxB];

            if (AreAabbsOverlapping(a, b))
            {
                // Build list of entries to then perform narrow phase on
                NarrowPhaseEntry newEntry;
                newEntry.handleA = shapeHandles[idxA];
                newEntry.handleB = shapeHandles[idxB];
                Assert(narrowPhaseCount < ArrayCount(narrowPhaseEntries));
                narrowPhaseEntries[narrowPhaseCount++] = newEntry;
            }
        }
    }

    // Perform narrow phase to generate contacts
    u32 contactCount = ProcessNarrowPhase(contacts, maxContacts,
        narrowPhaseEntries, narrowPhaseCount, collisionWorld);

    return contactCount;
}
