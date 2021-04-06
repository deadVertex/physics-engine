#include <cstdio>

#include <SDL.h>

#include "drawing.h"
#include "particle_physics_2d.h"

struct Camera
{
    vec2 position;
    vec2 velocity;
};

internal void UpdateCamera(Camera *camera, vec2 acceleration, f32 dt)
{
    f32 friction = 8.0f;
    camera->velocity = camera->velocity - camera->velocity * friction * dt;
    camera->velocity = camera->velocity + acceleration * dt;
    camera->position = camera->position + camera->velocity * dt;
}

global Camera g_Camera;

inline vec2 WorldSpaceToScreenSpace(vec2 p)
{
    f32 pixelsPerMeter = 80.0f;

    vec2 cameraP = g_Camera.position;
    p = p - cameraP;
    p = p * pixelsPerMeter;

    // Center camera
    p.x = p.x + 640.0f * 0.5f;
    p.y = p.y + 480.0f * 0.5f;
    p.y = 480.0f - p.y;

    return p;
}

static void DrawLines(SDL_Renderer *renderer, vec2 *vertices, u32 count)
{
    for (u32 i = 0; i < count; ++i)
    {
        vertices[i] = WorldSpaceToScreenSpace(vertices[i]);
    }

    SDL_RenderDrawLinesF(renderer, (const SDL_FPoint *)vertices, count);
}

// FIXME: Half dims are still in pixels!
static void DrawRects(
    SDL_Renderer *renderer, vec2 *centers, vec2 *halfDims, u32 count)
{
    SDL_FRect rects[256];
    Assert(count < ArrayCount(rects));

    for (u32 i = 0; i < count; ++i)
    {
        vec2 c = WorldSpaceToScreenSpace(centers[i]);
        rects[i].x = c.x - halfDims[i].x;
        rects[i].y = c.y - halfDims[i].y;
        rects[i].w = halfDims[i].x * 2.0f;
        rects[i].h = halfDims[i].y * 2.0f;
    }

    SDL_RenderFillRectsF(renderer, rects, count);
}

// NOTE: Size is in pixels
static void DrawPoints(SDL_Renderer *renderer, vec2 *points, u32 count, f32 size)
{
    SDL_FRect rects[256];
    Assert(count < ArrayCount(rects));

    for (u32 i = 0; i < count; ++i)
    {
        vec2 c = WorldSpaceToScreenSpace(points[i]);
        rects[i].x = c.x - size * 0.5f;
        rects[i].y = c.y - size * 0.5f;
        rects[i].w = size;
        rects[i].h = size;
    }

    SDL_RenderFillRectsF(renderer, rects, count);
}

static void RenderKinematicBodies(
    SDL_Renderer *renderer, KinematicsEngine *kinematicsEngine)
{
    vec2 vertices[128];
    for (u32 i = 0; i < kinematicsEngine->count; ++i)
    {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        vertices[0] = kinematicsEngine->position[i];
        vertices[1] = kinematicsEngine->position[i] + Vec2(0.5f, 0);
        DrawLines(renderer, vertices, 2);

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        vertices[0] = kinematicsEngine->position[i];
        vertices[1] = kinematicsEngine->position[i] + Vec2(0, 0.5f);
        DrawLines(renderer, vertices, 2);
    }
}

static void RenderBoxes(SDL_Renderer *renderer, Box *boxes, u32 count)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    vec2 vertices[128];
    for (u32 i = 0; i < count; ++i)
    {
        u32 count = GenerateBoxVertices(vertices, ArrayCount(vertices),
            boxes[i].center, boxes[i].halfDims, 0.0f);
        DrawLines(renderer, vertices, count);

        vec2 center = WorldSpaceToScreenSpace(boxes[i].center);
        SDL_RenderDrawPointsF(renderer, (const SDL_FPoint *)&center, 1);
    }
}

static void RenderCircles(SDL_Renderer *renderer, Circle *circles, u32 count)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);

    vec2 vertices[128];
    for (u32 i = 0; i < count; ++i)
    {
        u32 count = GenerateCircleVertices(vertices, ArrayCount(vertices),
            circles[i].center, circles[i].radius, 15);
        DrawLines(renderer, vertices, count);

        vec2 center = WorldSpaceToScreenSpace(circles[i].center);
        SDL_RenderDrawPointsF(renderer, (const SDL_FPoint *)&center, 1);
    }
}

static void RenderAxis(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);

    vec2 vertices[4];
    f32 scale = 100.0f;
    vertices[0] = Vec2(-scale, 0);
    vertices[1] = Vec2(scale, 0);
    vertices[2] = Vec2(0, -scale);
    vertices[3] = Vec2(0, scale);
    DrawLines(renderer, vertices, 2);
    DrawLines(renderer, vertices + 2, 2);
}

static void RenderGrid(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);

    f32 scale = 100.0f;
    f32 stepSize = 1.0f;

    for (f32 y = -scale; y <= scale; y += stepSize)
    {
        vec2 vertices[2];
        vertices[0] = Vec2(-scale, y);
        vertices[1] = Vec2(scale, y);
        DrawLines(renderer, vertices, 2);
    }

    for (f32 x = -scale; x <= scale; x += stepSize)
    {
        vec2 vertices[2];
        vertices[0] = Vec2(x, -scale);
        vertices[1] = Vec2(x, scale);
        DrawLines(renderer, vertices, 2);
    }
}

inline void DrawSupport(SDL_Renderer *renderer, vec2 origin, vec2 axis, f32 t)
{
    vec2 perp = Perpendicular(axis);

    vec2 vertices[2];
    vertices[0] = origin + axis * t + perp * 5.0f;
    vertices[1] = origin + axis * t - perp * 5.0f;

    DrawLines(renderer, vertices, 2);

    vec2 p = origin + axis * t;
    vec2 h = Vec2(10, 10);
    DrawRects(renderer, &p, &h, 1);
}

/*
internal void DrawSATAxis(
    SDL_Renderer *renderer, vec2 origin, vec2 axis, SATIntervals intervals)
{
    vec2 vertices[2];

    vertices[0] = origin + axis * 5.0f;
    vertices[1] = origin - axis * 5.0f;

    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    DrawLines(renderer, vertices, 2);

    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    DrawSupport(renderer, origin, axis, intervals.values[0]);

    SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
    DrawSupport(renderer, origin, axis, intervals.values[1]);

    SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
    DrawSupport(renderer, origin, axis, intervals.values[2]);

    SDL_SetRenderDrawColor(renderer, 255, 100, 255, 255);
    DrawSupport(renderer, origin, axis, intervals.values[3]);
}

internal void DrawBoxSAT(SDL_Renderer *renderer)
{
    Box boxA;
    boxA.center = Vec2(-10, 1.0);
    boxA.halfDims = Vec2(0.8, 0.5);
    f32 orientationA = PI * 0.25f;

    Box boxB;
    boxB.center = Vec2(-10, 0.0);
    boxB.halfDims = Vec2(1.5, 0.2);
    f32 orientationB = 0.0f;
    {
        // Draw box A
        vec2 vertices[16];
        u32 count = GenerateBoxVertices(vertices, ArrayCount(vertices),
            boxA.center, boxA.halfDims, orientationA);
        DrawLines(renderer, vertices, count);
    }

    {
        // Draw box B
        vec2 vertices[16];
        u32 count = GenerateBoxVertices(vertices, ArrayCount(vertices),
            boxB.center, boxB.halfDims, orientationB);
        DrawLines(renderer, vertices, count);
    }

    vec2 boxAVertices[4];
    GetBoxVertices(boxAVertices, ArrayCount(boxAVertices), boxA.center,
        boxA.halfDims, orientationA);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    DrawPoints(renderer, boxAVertices, 4, 4.0f);

    vec2 boxBVertices[4];
    GetBoxVertices(boxBVertices, ArrayCount(boxBVertices), boxB.center,
        boxB.halfDims, orientationB);
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    DrawPoints(renderer, boxBVertices, 4, 4.0f);

    // Draw test axis
    vec2 axis[4];
    axis[0] = RotationMatrix(orientationA)* Vec2(1, 0);
    axis[1] = RotationMatrix(orientationA)* Vec2(0, 1);
    axis[2] = RotationMatrix(orientationB)* Vec2(1, 0);
    axis[3] = RotationMatrix(orientationB)* Vec2(0, 1);

    // Make everything relative to boxA.center
    for (u32 i = 0; i < 4; ++i)
    {
        boxAVertices[i] = boxAVertices[i] - boxA.center;
        boxBVertices[i] = boxBVertices[i] - boxA.center;
    }

    f32 minPen = F32_MAX;
    vec2 collisionNormal = {};
    for (u32 i = 0; i < 4; ++i)
    {
        SATIntervals intervals = CalculateIntervals(
            boxA.center, axis[i], boxAVertices, 4, boxBVertices, 4);

        f32 pen = Sat2(intervals.values[0], intervals.values[1],
            intervals.values[2], intervals.values[3]);
        if (pen > 0.0f)
        {
            if (pen < minPen)
            {
                minPen = pen;
                collisionNormal = axis[i];
            }
        }

        if (i == 3)
        {
            for (u32 j = 0; j < 4; ++j)
            {
                printf("intervals[%d] = %g\n", j, intervals.values[j]);
            }
            printf("pen[%d] = %g\n", i, pen);
            DrawSATAxis(renderer, boxA.center, axis[i], intervals);
            vec2 p = boxA.center;
            vec2 h = Vec2(10, 10);

            SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
            DrawRects(renderer, &p, &h, 1);
        }
    }
}
*/

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("2D Physics Engine",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);

    if (window == NULL)
    {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    printf("2D Physics engine\n");

    Box boxes[16];
    Circle circles[16];
    CollisionWorld collisionWorld = CreateCollisionWorld(
            boxes, ArrayCount(boxes), circles, ArrayCount(circles));

    u32 box0 = AddBox(&collisionWorld, Vec2(0, 0), Vec2(2.5f, 0.1f), 0.0f);
    u32 box1 = AddBox(&collisionWorld, Vec2(0, 0), Vec2(0.25f, 0.25f), 0.0f);
    u32 circle0 = AddCircle(&collisionWorld, Vec2(0, 0), 0.25f);
    u32 circle1 = AddCircle(&collisionWorld, Vec2(0, 0), 0.25f);

    /*collisionWorld.boxes[1].center = Vec2(-2.5f, 2.5f);
    collisionWorld.boxes[1].halfDims = Vec2(0.2f, 5) * 0.5f;
    collisionWorld.boxes[2].center = Vec2(2.5f, 2.5f);
    collisionWorld.boxes[2].halfDims = Vec2(0.2f, 5) * 0.5f;*/

    KinematicsEngine kinematicsEngine = {};
    kinematicsEngine.position[0] = Vec2(-1, 2.5);
    kinematicsEngine.acceleration[0] = Vec2(0.2f, -0.5f);
    kinematicsEngine.invMass[0] = 1.0f / 5.0f;
    kinematicsEngine.forceAccumulation[0] = Vec2(-10.0, 0);
    kinematicsEngine.position[1] = Vec2(-0.5, 2.5);
    kinematicsEngine.acceleration[1] = Vec2(-0.1f, -0.5f);
    kinematicsEngine.invMass[1] = 1.0f / 15.0f;
    kinematicsEngine.forceAccumulation[1] = Vec2(20.0, 0);
    kinematicsEngine.position[2] = Vec2(0, 5);
    kinematicsEngine.acceleration[2] = Vec2(0, 0);
    kinematicsEngine.invMass[2] = 1.0f / 25.0f;
    kinematicsEngine.forceAccumulation[2] = Vec2(0, -1);
    kinematicsEngine.count = 3;

    kinematicsEngine.collisionShapeBindings[0].bodyIndex = 0;
    kinematicsEngine.collisionShapeBindings[0].shapeHandle = circle0;
    kinematicsEngine.collisionShapeBindings[1].bodyIndex = 1;
    kinematicsEngine.collisionShapeBindings[1].shapeHandle = circle1;
    kinematicsEngine.collisionShapeBindings[2].bodyIndex = 2;
    kinematicsEngine.collisionShapeBindings[2].shapeHandle = box1;
    kinematicsEngine.bindingCount = 3;

    g_Camera.position = Vec2(0, 2.5f);

    b32 isRunning = true;
    f32 orientation = 0.0f;
    while (isRunning)
    {
        vec2 cameraAcceleration = {};
        f32 cameraSpeed = 160.0f;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                isRunning = false;
            }
        }

        const u8 *keyState = SDL_GetKeyboardState(NULL);
        if (keyState[SDL_GetScancodeFromKey(SDLK_w)])
        {
            cameraAcceleration.y += cameraSpeed;
        }
        if (keyState[SDL_GetScancodeFromKey(SDLK_s)])
        {
            cameraAcceleration.y -= cameraSpeed;
        }
        if (keyState[SDL_GetScancodeFromKey(SDLK_a)])
        {
            cameraAcceleration.x -= cameraSpeed;
        }
        if (keyState[SDL_GetScancodeFromKey(SDLK_d)])
        {
            cameraAcceleration.x += cameraSpeed;
        }

        f32 dt = 0.016f;

        orientation += 0.1f * dt;
        if (orientation > PI * 2.0f)
        {
            orientation -= PI * 2.0f;
        }

        UpdateCamera(&g_Camera, cameraAcceleration, dt);

        for (u32 i = 0; i < kinematicsEngine.count; ++i)
        {
            kinematicsEngine.acceleration[i] = Vec2(0, -9.8f);
        }

        UpdateAcceleration(kinematicsEngine.acceleration,
            kinematicsEngine.forceAccumulation, kinematicsEngine.invMass,
            kinematicsEngine.count);
        Integrate2D(kinematicsEngine.position, kinematicsEngine.velocity,
            kinematicsEngine.acceleration, kinematicsEngine.count, dt);
        ResolveCollisions(&kinematicsEngine, &collisionWorld);

        SDL_SetRenderDrawColor(renderer, 45, 45, 45, 255);
        SDL_RenderClear(renderer);
        RenderGrid(renderer);
        RenderAxis(renderer);
        RenderBoxes(renderer, collisionWorld.boxes, collisionWorld.boxCount);
        RenderKinematicBodies(renderer, &kinematicsEngine);
        RenderCircles(
            renderer, collisionWorld.circles, collisionWorld.circleCount);
        //DrawBoxSAT(renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
