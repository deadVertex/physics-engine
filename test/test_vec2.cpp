#include <gtest/gtest.h>

#include "math_lib.h"
#undef internal

TEST(vec2, Create)
{
    vec2 v = Vec2(1.0f, 2.0f);
    ASSERT_EQ(v.x, 1.0f);
    ASSERT_EQ(v.y, 2.0f);
}

TEST(vec2, Add)
{
    vec2 v = Vec2(1.0f, 2.0f) + Vec2(3.0f, 4.0f);
    ASSERT_EQ(v.x, 4.0f);
    ASSERT_EQ(v.y, 6.0f);
}

TEST(vec2, Subtract)
{
    vec2 v = Vec2(3.0f, 4.0f) - Vec2(1.0f, 2.0f);
    ASSERT_EQ(v.x, 2.0f);
    ASSERT_EQ(v.y, 2.0f);
}

TEST(vec2, Multiply)
{
    vec2 v = Vec2(1.0f, 2.0f) * 2.0f;
    ASSERT_EQ(v.x, 2.0f);
    ASSERT_EQ(v.y, 4.0f);
}
