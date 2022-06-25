#include "clock.h"

float now()
{
    return (float)SDL_GetTicks() / 1000.0;
}

float nowScaled(struct Epoch t)
{ 
    return t.scale * now();
}
