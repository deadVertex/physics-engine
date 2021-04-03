#pragma once

#include "math_lib.h"

struct Box
{
    vec2 center;
    vec2 halfDims;
};

struct Circle
{
    vec2 center;
    f32 radius;
};

struct KinematicsEngine
{
    vec2 position[16];
    vec2 velocity[16];
    vec2 acceleration[16];
    u32 collisionShape[16]; // Only circles supported for now
    u32 count;
};

struct CollisionWorld
{
    Box boxes[16];
    u32 boxCount;

    Circle circles[16];
    u32 circleCount;
};

void Integrate2D(
    vec2 *positions, vec2 *velocities, vec2 *accelerations, u32 count, f32 dt);

struct OverlapEvent
{
    u32 circleIndex;
    vec2 normal;
};

u32 DetectCollisions(
    CollisionWorld *collisionWorld, OverlapEvent *events, u32 maxEvents);

void ResolveCollisions(
    KinematicsEngine *kinematicsEngine, CollisionWorld *collisionWorld);

inline f32 Sat(f32 p0, f32 r0, f32 p1, f32 r1)
{
    f32 a = p0 - r0;
    f32 b = p0 + r0;
    f32 c = p1 - r1;
    f32 d = p1 + r1;

    f32 pen = 0.0f;
    if (a < c)
    {
        if (b < c)
        {
            // ------- a == b --- c === d
        }
        else
        {
            pen = b - c;
            // or ------- a ==== c #### b === d
        }
    }
    else
    {
        if (d < a)
        {
            // ----------- c ===== d --- a ===== b
        }
        else
        {
            pen = d - a;
            // ----------- c === a ## d ==== b
        }
    }

    return pen;
}

inline f32 SquaredDistanceBetweenPointAndAabb(vec2 p, vec2 min, vec2 max)
{
    f32 sqDist = 0.0f;
    sqDist += (p.x < min.x) ? Square(min.x - p.x) : 0.0f;
    sqDist += (p.x > max.x) ? Square(p.x - max.x) : 0.0f;
    sqDist += (p.y < min.y) ? Square(min.y - p.y) : 0.0f;
    sqDist += (p.y > max.y) ? Square(p.y - max.y) : 0.0f;
    return sqDist;
}
