#include "input.h"

int checkQuit()
{
    int quit = 0;
    SDL_Event event;

    if (SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                quit = 1;
                break;

            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_q)
                {
                    quit = 1;
                }

            default:
                break;
        }
    }

    return quit;
}
