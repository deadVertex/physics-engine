#include "drawing.h"

u32 GenerateCircleVertices(
    vec2 *vertices, u32 maxVertices, vec2 center, f32 radius, u32 segmentCount)
{
    u32 count = 0;

    for (u32 i = 0; i <= segmentCount; ++i)
    {
        f32 t0 = ((f32)i / (f32)segmentCount) * 2.0f * PI;
        // f32 t1 = ((f32)(i + 1) / (f32)segmentCount) * 2.0f * PI;

        f32 x0 = Cos(t0) * radius;
        f32 y0 = Sin(t0) * radius;
        // f32 x1 = Cos(t1) * radius;
        // f32 y1 = Sin(t1) * radius;

        vec2 p0 = center + Vec2(x0, y0);
        // vec2 p1 = center + Vec2(x1, y1);

        if (count + 1 <= maxVertices)
        {
            vertices[count++] = p0;
            // vertices[count++] = p1;
        }
        else
        {
            break;
        }
    }

    return count;
}

u32 GenerateBoxVertices(vec2 *vertices, u32 maxVertices, vec2 center,
    vec2 halfDims, f32 orientation)
{
    (void)orientation;

    u32 count = 0;

    if (maxVertices >= 5)
    {
        mat2 rotation = RotationMatrix(orientation);
        vertices[0] = center + rotation * Vec2(-halfDims.x, -halfDims.y);
        vertices[1] = center + rotation * Vec2(halfDims.x, -halfDims.y);
        vertices[2] = center + rotation * Vec2(halfDims.x, halfDims.y);
        vertices[3] = center + rotation * Vec2(-halfDims.x, halfDims.y);
        vertices[4] = center + rotation * Vec2(-halfDims.x, -halfDims.y);
        count = 5;
    }

    return count;
}
