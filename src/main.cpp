#include <cstdio>

#include <SDL.h>

#include "drawing.h"
#include "particle_physics_2d.h"

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

        vec2 center = WorldSpaceToScreenSpace(physics->position[i]);
        SDL_RenderDrawPointsF(renderer, (const SDL_FPoint *)&center, 1);
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

        vec2 center = WorldSpaceToScreenSpace(boxes[i].center);
        SDL_RenderDrawPointsF(renderer, (const SDL_FPoint *)&center, 1);
    }
}

inline f32 SquaredDistanceBetweenPointAndAabb(vec2 p, vec2 min, vec2 max)
{
    f32 sqDist = 0.0f;
    sqDist += (p.x < min.x) ? Square(min.x - p.x) : 0.0f;
    sqDist += (p.x > max.x) ? Square(p.x - max.x) : 0.0f;
    sqDist += (p.y < min.y) ? Square(min.y - p.y) : 0.0f;
    sqDist += (p.y > max.y) ? Square(p.y - max.y) : 0.0f;
    return sqDist;
}

struct SatResult
{
    f32 pen;
    vec2 normal;
};

inline SatResult SAT(vec2 p, f32 radius, vec2 center, vec2 halfDims)
{
    f32 pen[4];
    pen[0] = (p.x + radius) - (center.x - halfDims.x);
    pen[1] = (center.x + halfDims.x) - (p.x - radius);
    pen[2] = (p.y + radius) - (center.y - halfDims.y);
    pen[3] = (center.y + halfDims.y) - (p.y - radius);

    vec2 normals[4];
    normals[0] = Vec2(-1, 0);
    normals[1] = Vec2(1, 0);
    normals[2] = Vec2(0, -1);
    normals[3] = Vec2(0, 1);

    SatResult result = {};
    for (u32 i = 0; i < 4; ++i)
    {
        if (pen[i] < 0.0f)
        {
            if (pen[i] < result.pen)
            {
                result.pen = pen[i];
                result.normal = normals[i];
            }
        }
        else
        {
            result = {};
            break;
        }
    }

    return result;
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
            vec2 p = particlePhysics.position[i];
            f32 r = 0.25f;
            for (u32 boxIndex = 0; boxIndex < ArrayCount(boxes); ++boxIndex)
            {
                vec2 c = boxes[boxIndex].center;
                vec2 h = boxes[boxIndex].halfDims;

                f32 x = Sat(p.x, r, c.x, h.x);
                f32 y = Sat(p.y, r, c.y, h.y);
                if (x > 0.0f && y > 0.0f)
                {
                    // TODO: Use axis of particle velocity
                    if (x < y)
                    {
                        particlePhysics.velocity[i].x =
                            -particlePhysics.velocity[i].x;
                    }
                    else
                    {
                        particlePhysics.velocity[i].y =
                            -particlePhysics.velocity[i].y;
                    }
                }
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
