#include <cstdio>

#include <SDL.h>

#include "particle_physics_2d.h"
#include "drawing.h"

inline vec2 WorldSpaceToScreenSpace(vec2 p)
{
    f32 pixelsPerMeter = 80.0f;

    vec2 cameraP = Vec2(0, 2.5f);
    p = p - cameraP;
    p = p * pixelsPerMeter;

    // Center camera
    p.x = p.x + 640.0f * 0.5f;
    p.y = p.y + 480.0f * 0.5f;
    p.y = 480.0f - p.y;

    return p;
}

static void RenderParticles(ParticlePhysics2D *physics, SDL_Renderer *renderer)
{
    f32 width = 4.0f;
    f32 height = 4.0f;

    SDL_FRect rects[64];
    for (u32 i = 0; i < physics->count; ++i)
    {
        rects[i].x = physics->position[i].x - width * 0.5f;
        rects[i].y = physics->position[i].y - height * 0.5f;
        rects[i].w = width;
        rects[i].h = height;
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRectsF(renderer, rects, physics->count);
}

static void DrawLines(SDL_Renderer *renderer, vec2 *vertices, u32 count)
{
    for (u32 i = 0; i < count; ++i)
    {
        vertices[i] = WorldSpaceToScreenSpace(vertices[i]);
    }

    SDL_RenderDrawLinesF(renderer, (const SDL_FPoint *)vertices, count);
}

static void RenderParticlesAsCircles(
    ParticlePhysics2D *physics, SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    vec2 vertices[128];
    for (u32 i = 0; i < physics->count; ++i)
    {
        u32 count = GenerateCircleVertices(
            vertices, ArrayCount(vertices), physics->position[i], 0.25f, 15);
        DrawLines(renderer, vertices, count);
    }
}

struct Box
{
    vec2 center;
    vec2 halfDims;
};

static void RenderBoxes(SDL_Renderer *renderer, Box *boxes, u32 count)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);

    vec2 vertices[128];
    for (u32 i = 0; i < count; ++i)
    {
        u32 count = GenerateBoxVertices(vertices, ArrayCount(vertices),
            boxes[i].center, boxes[i].halfDims, 0.0f);
        DrawLines(renderer, vertices, count);
    }
}

int main(int argc, char **argv)
{
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

    ParticlePhysics2D particlePhysics = {};
    particlePhysics.position[0] = Vec2(-1, 2.5);
    particlePhysics.acceleration[0] = Vec2(0.2f, -0.5f);
    particlePhysics.position[1] = Vec2(-0.5, 2.5);
    particlePhysics.acceleration[1] = Vec2(-0.1f, -0.5f);
    particlePhysics.count = 2;

    Box boxes[3];
    boxes[0].center = Vec2(0, 0);
    boxes[0].halfDims = Vec2(5, 0.2f) * 0.5f;
    boxes[1].center = Vec2(-2.5f, 2.5f);
    boxes[1].halfDims = Vec2(0.2f, 5) * 0.5f;
    boxes[2].center = Vec2(2.5f, 2.5f);
    boxes[2].halfDims = Vec2(0.2f, 5) * 0.5f;

    while (1)
    {
        SDL_Event event;
        if (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                break;
            }
        }

        Integrate2D(particlePhysics.position, particlePhysics.velocity,
            particlePhysics.acceleration, particlePhysics.count, 0.016f);
        
        // Super dumb collision detection
        for (u32 i = 0; i < particlePhysics.count; ++i)
        {
            if (particlePhysics.position[i].x < -2.5f ||
                particlePhysics.position[i].x > 2.5f)
            {
                particlePhysics.velocity[i].x = -particlePhysics.velocity[i].x;
            }

            if (particlePhysics.position[i].y < 0.0f ||
                particlePhysics.position[i].y > 5.0f)
            {
                particlePhysics.velocity[i].y = -particlePhysics.velocity[i].y;
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        RenderBoxes(renderer, boxes, ArrayCount(boxes));
        RenderParticlesAsCircles(&particlePhysics, renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
