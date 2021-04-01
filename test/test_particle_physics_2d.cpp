#include <gtest/gtest.h>

#include "particle_physics_2d.h"

TEST(ParticlePhysics2D, Integrate)
{
    vec2 position = Vec2(1.0f, 2.0f);
    vec2 velocity = Vec2(0.0f, 0.0f);
    vec2 acceleration = Vec2(2.0f, 1.0f);
    Integrate2D(&position, &velocity, &acceleration, 1, 1.0f);

    // FIXME: Don't assert equality, going to get bitten by floating point
    ASSERT_EQ(velocity.x, 2.0f);
    ASSERT_EQ(velocity.y, 1.0f);
    ASSERT_EQ(position.x, 3.0f);
    ASSERT_EQ(position.y, 3.0f);
}
