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

/*
struct OverlapEvent
{
    u32 circleIndex;
    vec2 normal;
};

u32 DetectCollisions(
    CollisionWorld *collisionWorld, OverlapEvent *events, u32 maxEvents);
*/

void ResolveCollisions(
    KinematicsEngine *kinematicsEngine, CollisionWorld *collisionWorld);

/*
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

inline f32 Sat2(f32 a, f32 b, f32 c, f32 d)
{
    if (a > b)
    {
        f32 temp = a;
        a = b;
        b = temp;
    }

    if (c > d)
    {
        f32 temp = c;
        c = d;
        d = temp;
    }

    //   D          B   C             A
    // --|----|----|----|----|----|----|----|----|--
    //  -2  -1.5  -1  -0.5   0   0.5   1   1.5   2
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

// TODO: Consolidate with GenerateBoxVertices
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

struct SATIntervals
{
    f32 values[4];
};

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


inline SATIntervals CalculateIntervals(vec2 origin, vec2 axis, vec2 *verticesA,
    u32 verticesCountA, vec2 *verticesB, u32 verticesCountB)
{
    SATIntervals intervals;
    intervals.values[0] = Support(verticesA, verticesCountA, axis);
    intervals.values[1] = -Support(verticesA, verticesCountA, -axis);
    intervals.values[2] = Support(verticesB, verticesCountB, axis);
    intervals.values[3] = -Support(verticesB, verticesCountB, -axis);

    return intervals;
}
*/
