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

    CollisionWorld collisionWorld = {};
    collisionWorld.boxes[0].center = Vec2(0, 0);
    collisionWorld.boxes[0].halfDims = Vec2(5, 0.2f) * 0.5f;
    collisionWorld.boxes[1].center = Vec2(-2.5f, 2.5f);
    collisionWorld.boxes[1].halfDims = Vec2(0.2f, 5) * 0.5f;
    collisionWorld.boxes[2].center = Vec2(2.5f, 2.5f);
    collisionWorld.boxes[2].halfDims = Vec2(0.2f, 5) * 0.5f;
    collisionWorld.boxCount = 3;

    collisionWorld.circles[0].center = Vec2(0, 0);
    collisionWorld.circles[0].radius = 0.25f;
    collisionWorld.circles[1].center = Vec2(0, 0);
    collisionWorld.circles[1].radius = 0.25f;
    collisionWorld.circleCount = 2;

    KinematicsEngine kinematicsEngine = {};
    kinematicsEngine.position[0] = Vec2(-1, 2.5);
    kinematicsEngine.acceleration[0] = Vec2(0.2f, -0.5f);
    kinematicsEngine.collisionShape[0] = 0;
    kinematicsEngine.invMass[0] = 1.0f / 5.0f;
    kinematicsEngine.forceAccumulation[0] = Vec2(-10.0, 0);
    kinematicsEngine.position[1] = Vec2(-0.5, 2.5);
    kinematicsEngine.acceleration[1] = Vec2(-0.1f, -0.5f);
    kinematicsEngine.collisionShape[1] = 1;
    kinematicsEngine.invMass[1] = 1.0f / 15.0f;
    kinematicsEngine.forceAccumulation[1] = Vec2(20.0, 0);
    kinematicsEngine.count = 2;

    g_Camera.position = Vec2(0, 2.5f);

    b32 isRunning = true;
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

        const u8* keyState = SDL_GetKeyboardState(NULL);
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
        {
            vec2 vertices[16];
            u32 count = GenerateBoxVertices(vertices, ArrayCount(vertices),
                    Vec2(0, 2.5), Vec2(0.5, 0.5), PI * 0.5f);
            DrawLines(renderer, vertices, count);
        }
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
