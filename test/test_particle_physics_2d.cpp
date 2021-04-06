#include <gtest/gtest.h>

#include "particle_physics_2d.h"
#undef internal

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

TEST(ParticlePhysics2D, CalculateAcceleration)
{
    vec2 force = Vec2(5, 10);
    f32 invMass = 1.0f / 5.0f;

    vec2 acceleration = {};
    UpdateAcceleration(&acceleration, &force, &invMass, 1);

    // a = F / m
    ASSERT_NEAR(1.0f, acceleration.x, EPSILON);
    ASSERT_NEAR(2.0f, acceleration.y, EPSILON);
}

/*
TEST(CollisionWorld, ReturnsNoEventsIfNoCollisions)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));

    AddBox(&collisionWorld, Vec2(-10, 0), Vec2(0.25f, 0.25f), 0.0f);
    AddCircle(&collisionWorld, Vec2(10, 0), 1.0f);

    OverlapEvent events[16] = {};
    ASSERT_EQ(DetectCollisions(&collisionWorld, events, ArrayCount(events)), 0);
}
*/

/*
TEST(CollisionWorld, DetectsCollision)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));
    AddBox(&collisionWorld, Vec2(0, 0), Vec2(0.5f, 0.5f), 0.0f);
    AddCircle(&collisionWorld, Vec2(0, 1), 0.75f);

    OverlapEvent events[16] = {};
    ASSERT_EQ(1, DetectCollisions(&collisionWorld, events, ArrayCount(events)));
}
*/

TEST(ResolveCollisions, SyncCollisionWorldWithKinematicsEngine)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));
    u32 circle = AddCircle(&collisionWorld, Vec2(0, 0), 0.75f);

    KinematicsEngine kinematicsEngine = {};
    kinematicsEngine.position[0] = Vec2(10, 20);
    kinematicsEngine.count = 1;

    kinematicsEngine.collisionShapeBindings[0].shapeHandle = circle;
    kinematicsEngine.collisionShapeBindings[0].bodyIndex = 0;
    kinematicsEngine.bindingCount = 1;

    ResolveCollisions(&kinematicsEngine, &collisionWorld);

    ASSERT_NEAR(collisionWorld.circles[0].center.x, 10.0f, EPSILON);
    ASSERT_NEAR(collisionWorld.circles[0].center.y, 20.0f, EPSILON);
}

/*
TEST(ResolveCollisions, UpdatesVelocity)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));
    AddCircle(&collisionWorld, Vec2(0, 0), 0.75f);
    AddBox(&collisionWorld, Vec2(0, 0), Vec2(0.5f, 0.5f), 0.0f);

    KinematicsEngine kinematicsEngine = {};
    kinematicsEngine.position[0] = Vec2(0, 1.0);
    kinematicsEngine.velocity[0] = Vec2(0, -10.0);
    kinematicsEngine.count = 1;

    // TODO: Remove duplicate code?
    kinematicsEngine.circleBindings[0].shapeIndex = 0;
    kinematicsEngine.circleBindings[0].bodyIndex = 0;
    kinematicsEngine.circleBindingCount = 1;

    ResolveCollisions(&kinematicsEngine, &collisionWorld);

    ASSERT_NEAR(kinematicsEngine.velocity[0].y, 10.0f, EPSILON);
}
*/
