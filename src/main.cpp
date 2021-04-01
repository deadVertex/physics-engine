#include <cstdio>

#include <SDL.h>

int main(int argc, char **argv)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "Failed to initialize SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window =
        SDL_CreateWindow("2D Physics Engine", SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);

    if (window == NULL)
    {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        return 1;
    }
    printf("2D Physics engine\n");

    SDL_Delay(3000);

    SDL_DestroyWindow(window);

    SDL_Quit();
    return 0;
}
