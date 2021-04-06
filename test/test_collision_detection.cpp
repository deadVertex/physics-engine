#include <gtest/gtest.h>

#include "collision_detection.h"
#undef internal

TEST(Circle, CalculateAabb)
{
    Aabb aabb = CalculateCircleAabb(Vec2(1, 2), 0.5f);
    ASSERT_NEAR(0.5f, aabb.min.x, EPSILON);
    ASSERT_NEAR(1.5f, aabb.min.y, EPSILON);
    ASSERT_NEAR(1.5f, aabb.max.x, EPSILON);
    ASSERT_NEAR(2.5f, aabb.max.y, EPSILON);
}

TEST(Box, CalculateAabb)
{
    Aabb aabb = CalculateBoxAabb(Vec2(1, 2), Vec2(1.0f, 0.5f), PI * 0.5f);
    ASSERT_NEAR(0.5f, aabb.min.x, EPSILON);
    ASSERT_NEAR(1.0f, aabb.min.y, EPSILON);
    ASSERT_NEAR(1.5f, aabb.max.x, EPSILON);
    ASSERT_NEAR(3.0f, aabb.max.y, EPSILON);
}

TEST(CollisionWorld, AddBox)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));

    u32 box = AddBox(&collisionWorld, Vec2(0, 0), Vec2(0.5, 0.5), 0.0f);
    ASSERT_EQ(CollisionShape_Box, GetCollisionShapeHandleType(box));
    ASSERT_EQ(0, GetCollisionShapeHandleIndex(box));

    u32 circle = AddCircle(&collisionWorld, Vec2(0, 0), 0.5f);
    ASSERT_EQ(CollisionShape_Circle, GetCollisionShapeHandleType(circle));
    ASSERT_EQ(0, GetCollisionShapeHandleIndex(circle));
}

TEST(CollisionWorld, GeneratesNoContactsIfNoCollision)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));

    AddBox(&collisionWorld, Vec2(0, 0), Vec2(0.5, 0.5), 0.0f);

    AddCircle(&collisionWorld, Vec2(0, 2), 0.5f);

    Contact contacts[4];
    ASSERT_EQ(
        0, GenerateContacts(contacts, ArrayCount(contacts), &collisionWorld));
}

TEST(CollisionWorld, GeneratesContacts)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));

    u32 boxA = AddBox(&collisionWorld, Vec2(0, 0), Vec2(0.5, 0.5), 0.0f);
    u32 boxB = AddBox(&collisionWorld, Vec2(0, -1), Vec2(0.5, 0.75), 0.0f);

    Contact contacts[4];
    ASSERT_EQ(
        1, GenerateContacts(contacts, ArrayCount(contacts), &collisionWorld));
    ASSERT_EQ(boxA, contacts[0].shapeHandles[0]);
    ASSERT_EQ(boxB, contacts[0].shapeHandles[1]);
}

TEST(CollisionWorld, ProcessNarrowPhase)
{
    Box boxes[4];
    Circle circles[4];
    CollisionWorld collisionWorld = CreateCollisionWorld(
        boxes, ArrayCount(boxes), circles, ArrayCount(circles));

    NarrowPhaseEntry narrowPhase;
    narrowPhase.handleA =
        AddBox(&collisionWorld, Vec2(2, 0.5), Vec2(0.5, 0.5), 0.0f);
    narrowPhase.handleB =
        AddBox(&collisionWorld, Vec2(2, 0), Vec2(1, 0.5), 0.0f);

    Contact contacts[4];
    u32 count = ProcessNarrowPhase(
        contacts, ArrayCount(contacts), &narrowPhase, 1, &collisionWorld);
    ASSERT_EQ(1, count);
}

TEST(CollisionWorld, BoxNarrowPhase)
{
    /*
     *     +---+
     *     | A |
     *  +--+---+--+
     *  |    B    |
     *  +---------+
     */

    Box boxA = {};
    boxA.center = Vec2(2, 0.5);
    boxA.halfDims = Vec2(0.5, 0.5);

    Box boxB = {};
    boxB.center = Vec2(2, 0);
    boxB.halfDims = Vec2(1, 0.5);

    Contact contact = {};
    ASSERT_TRUE(BoxVsBoxNarrowPhase(&contact, boxA, boxB));
    ASSERT_NEAR(0.0f, contact.normal.x, EPSILON);
    ASSERT_NEAR(1.0f, contact.normal.y, EPSILON);
}

TEST(CollisionWorld, BoxNarrowPhaseWithOrientation)
{
    /*       x
     *     /   \
     *     \ A  \
     *      \   /
     *  +-----x---+
     *  |    B    |
     *  +---------+
     */

    Box boxA = {};
    boxA.center = Vec2(2, 0.5);
    boxA.halfDims = Vec2(0.5, 0.5);
    boxA.orientation = PI * 0.25f;

    Box boxB = {};
    boxB.center = Vec2(2, 0);
    boxB.halfDims = Vec2(1, 0.5);

    Contact contact = {};
    ASSERT_TRUE(BoxVsBoxNarrowPhase(&contact, boxA, boxB));
    ASSERT_NEAR(0.0f, contact.normal.x, EPSILON);
    ASSERT_NEAR(1.0f, contact.normal.y, EPSILON);
}
