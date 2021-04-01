#include <gtest/gtest.h>

#include "drawing.h"

TEST(Drawing, GenerateCircleVertices)
{
    vec2 vertices[32];
    u32 count = GenerateCircleVertices(
        vertices, ArrayCount(vertices), Vec2(1.0f, 2.0f), 1.5f, 4);
    ASSERT_EQ(count, 5);
    ASSERT_NEAR(vertices[0].x, 2.5f, EPSILON);
    ASSERT_NEAR(vertices[0].y, 2.0f, EPSILON);
    ASSERT_NEAR(vertices[1].x, 1.0f, EPSILON);
    ASSERT_NEAR(vertices[1].y, 3.5f, EPSILON);
}
