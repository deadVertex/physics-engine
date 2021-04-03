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

TEST(CollisionWorld, ReturnsNoEventsIfNoCollisions)
{
    CollisionWorld collisionWorld = {};
    collisionWorld.boxes[0].center = Vec2(0, 0);
    collisionWorld.boxes[0].halfDims = Vec2(0.5f, 0.5f);
    collisionWorld.circles[0].center = Vec2(10, 0);
    collisionWorld.circles[0].radius = 1.0f;
    collisionWorld.boxCount = 1;
    collisionWorld.circleCount = 1;

    OverlapEvent events[16] = {};
    ASSERT_EQ(DetectCollisions(&collisionWorld, events, ArrayCount(events)), 0);
}

TEST(CollisionWorld, DetectsCollision)
{
    CollisionWorld collisionWorld = {};
    collisionWorld.boxes[0].center = Vec2(0, 0);
    collisionWorld.boxes[0].halfDims = Vec2(0.5f, 0.5f);
    collisionWorld.circles[0].center = Vec2(0, 1);
    collisionWorld.circles[0].radius = 0.75f;
    collisionWorld.boxCount = 1;
    collisionWorld.circleCount = 1;

    OverlapEvent events[16] = {};
    ASSERT_EQ(1, DetectCollisions(&collisionWorld, events, ArrayCount(events)));
}

TEST(ResolveCollisions, SyncCollisionWorldWithKinematicsEngine)
{
    CollisionWorld collisionWorld = {};
    collisionWorld.circles[0].center = Vec2(0, 0);
    collisionWorld.circles[0].radius = 0.75f;
    collisionWorld.circleCount = 1;

    KinematicsEngine kinematicsEngine = {};
    kinematicsEngine.position[0] = Vec2(10, 20);
    kinematicsEngine.collisionShape[0] = 0;
    kinematicsEngine.count = 1;

    ResolveCollisions(&kinematicsEngine, &collisionWorld);

    ASSERT_NEAR(collisionWorld.circles[0].center.x, 10.0f, EPSILON);
    ASSERT_NEAR(collisionWorld.circles[0].center.y, 20.0f, EPSILON);
}

TEST(ResolveCollisions, UpdatesVelocity)
{
    CollisionWorld collisionWorld = {};
    collisionWorld.circles[0].center = Vec2(0, 0);
    collisionWorld.circles[0].radius = 0.75f;
    collisionWorld.circleCount = 1;
    collisionWorld.boxes[0].center = Vec2(0, 0);
    collisionWorld.boxes[0].halfDims = Vec2(0.5f, 0.5f);
    collisionWorld.boxCount = 1;

    KinematicsEngine kinematicsEngine = {};
    kinematicsEngine.position[0] = Vec2(0, 1.0);
    kinematicsEngine.velocity[0] = Vec2(0, -10.0);
    kinematicsEngine.collisionShape[0] = 0;
    kinematicsEngine.count = 1;

    ResolveCollisions(&kinematicsEngine, &collisionWorld);

    ASSERT_NEAR(kinematicsEngine.velocity[0].y, 10.0f, EPSILON);
}
