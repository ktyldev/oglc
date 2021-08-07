#include "input.h"

int checkQuit()
{
    SDL_Event event;

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) return 1;

    return 0;
}
