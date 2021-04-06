#pragma once

#include "math_lib.h"

enum
{
    CollisionShape_Circle = 0,
    CollisionShape_Box    = 1,
};

inline u32 BuildCollisionShapeHandle(u32 type, u32 idx)
{
    Assert(type < 0xFF); // TODO: Don't need all 8 bits
    Assert(idx < 0x1000); // TODO: Formalize limits
    u32 handle = (type << 24) | idx;
    return handle;
}

inline u32 GetCollisionShapeHandleIndex(u32 handle)
{
    u32 idx = (handle & 0x00FFFFFF);
    return idx;
}

inline u32 GetCollisionShapeHandleType(u32 handle)
{
    u32 type = ((handle & 0xFF000000) >> 24);
    return type;
}

struct Box
{
    vec2 center;
    vec2 halfDims;
    f32 orientation;
};

struct Circle
{
    vec2 center;
    f32 radius;
};

struct Aabb
{
    vec2 min;
    vec2 max;
};

struct CollisionWorld
{
    Box *boxes;
    u32 maxBoxes;
    u32 boxCount;

    Circle *circles;
    u32 maxCircles;
    u32 circleCount;
};

struct Contact
{
    u32 shapeHandles[2];
    vec2 point;
    vec2 normal;
    f32 pen;
};

CollisionWorld CreateCollisionWorld(
    Box *boxes, u32 boxCount, Circle *circles, u32 circleCount);

u32 AddBox(CollisionWorld *collisionWorld, vec2 center, vec2 halfDims,
    f32 orientation);
u32 AddCircle(CollisionWorld *collisionWorld, vec2 center, f32 radius);

u32 GenerateContacts(
    Contact *contacts, u32 maxContacts, CollisionWorld *collisionWorld);

b32 BoxVsBoxNarrowPhase(Contact *contact, Box boxA, Box boxB);

struct NarrowPhaseEntry
{
    u32 handleA;
    u32 handleB;
};

u32 ProcessNarrowPhase(Contact *contacts, u32 maxContacts,
    NarrowPhaseEntry *narrowPhaseEntries, u32 narrowPhaseCount,
    CollisionWorld *collisionWorld);

inline Aabb CalculateCircleAabb(vec2 center, f32 radius)
{
    Aabb result;
    result.min = center - Vec2(radius, radius);
    result.max = center + Vec2(radius, radius);
    return result;
}

inline Aabb CalculateBoxAabb(vec2 c, vec2 h, f32 orientation)
{
    mat2 r = RotationMatrix(orientation);
    vec2 u = r * Vec2(1, 0);
    vec2 v = r * Vec2(0, 1);

    vec2 vertices[4];
    vertices[0] = c + u * -h.x + v * -h.y;
    vertices[1] = c + u * h.x + v * -h.y;
    vertices[2] = c + u * h.x + v * h.y;
    vertices[3] = c + u * -h.x + v * h.y;

    Aabb result;
    result.min = vertices[0];
    result.max = vertices[0];
    for (u32 i = 1; i < 4; ++i)
    {
        result.min = Min(result.min, vertices[i]);
        result.max = Max(result.max, vertices[i]);
    }

    return result;
}

inline b32 AreAabbsOverlapping(Aabb a, Aabb b)
{
    b32 isOverlapping = (Max(a.min.x, b.min.x) <= Min(a.max.x, b.max.x)) &&
                        (Max(a.min.y, b.min.y) <= Min(a.max.y, b.max.y));

    return isOverlapping;
}
