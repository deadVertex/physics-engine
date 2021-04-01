#pragma once

#include "platform.h"
#include "math_lib.h"

u32 GenerateCircleVertices(
    vec2 *vertices, u32 maxVertices, vec2 center, f32 radius, u32 segmentCount);
