#pragma once

#include "math_lib.h"
#include "platform.h"

u32 GenerateCircleVertices(
    vec2 *vertices, u32 maxVertices, vec2 center, f32 radius, u32 segmentCount);

u32 GenerateBoxVertices(vec2 *vertices, u32 maxVertices, vec2 center,
    vec2 halfDims, f32 orientation);
