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

TEST(Drawing, GenerateBoxVertices)
{
    vec2 vertices[8];
    vec2 dimensions = Vec2(2.0f, 4.0f);
    u32 count = GenerateBoxVertices(vertices, ArrayCount(vertices),
            Vec2(1.0f, 2.0f), dimensions * 0.5f, 0.0f);
    ASSERT_EQ(count, 5);
    ASSERT_NEAR(vertices[1].x - vertices[0].x, dimensions.x, EPSILON);
}
