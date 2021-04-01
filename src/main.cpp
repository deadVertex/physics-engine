#include <cstdio>

#include <SDL.h>

#include "particle_physics_2d.h"

static void RenderParticles(ParticlePhysics2D *physics, SDL_Renderer *renderer)
{
    f32 width = 4.0f;
    f32 height = 4.0f;

    SDL_FRect rects[64];
    for (u32 i = 0; i < physics->particleCount; ++i)
    {
        Particle *particle = physics->particles + i;
        rects[i].x = particle->position.x - width * 0.5f;
        rects[i].y = particle->position.y - height * 0.5f;
        rects[i].w = width;
        rects[i].h = height;
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRectsF(renderer, rects, physics->particleCount);
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
    particlePhysics.particles[0].position = Vec2(20.0f, 20.0f);
    particlePhysics.particles[0].acceleration = Vec2(2.5f, 4.0f);
    particlePhysics.particles[1].position = Vec2(40.0f, 40.0f);
    particlePhysics.particles[1].acceleration = Vec2(-1.2f, 5.5f);
    particlePhysics.particleCount = 2;

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

        Integrate(&particlePhysics, 0.016f);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        RenderParticles(&particlePhysics, renderer);
        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
