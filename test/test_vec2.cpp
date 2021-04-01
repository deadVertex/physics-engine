#include <gtest/gtest.h>

TEST(vec2, add)
{
    float a = 1.0f;
    float b = 2.0f;
    float c = a + b;
    ASSERT_NEAR(c, 3.0f, 1.0e-11);
}
