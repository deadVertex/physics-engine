#include <gtest/gtest.h>

#include "math_lib.h"
#undef internal

TEST(Mat2, Construct)
{
    mat2 result = Mat2(1.0f, 2.0f, 3.0f, 4.0f);
    ASSERT_EQ(result.columns[0].x, 1.0f);
    ASSERT_EQ(result.columns[0].y, 2.0f);
    ASSERT_EQ(result.columns[1].x, 3.0f);
    ASSERT_EQ(result.columns[1].y, 4.0f);
}

TEST(Mat2, MultiplyVec2)
{
    /*
     *  | 1.0 3.0 |   | 1.0 |   | 1.0 + 6.0 |   |  7.0 |
     *  | 2.0 4.0 | x | 2.0 | = | 2.0 + 8.0 | = | 10.0 |
     *
     */
    mat2 a = Mat2(1.0f, 2.0f, 3.0f, 4.0f);
    vec2 b = Vec2(1.0f, 2.0f);

    vec2 c = a * b;
    ASSERT_NEAR(7.0, c.x, EPSILON);
    ASSERT_NEAR(10.0, c.y, EPSILON);
}

TEST(Mat2, Rotation)
{
    mat2 a = RotationMatrix(PI * 0.5f);
    vec2 b = Vec2(2.5f, 0.0f);

    vec2 c = a * b;
    ASSERT_NEAR(0.0f, c.x, EPSILON);
    ASSERT_NEAR(2.5f, c.y, EPSILON);
}
