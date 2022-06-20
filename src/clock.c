#include "clock.h"

float now(struct Epoch t) 
{ 
    return t.speed * (float)SDL_GetTicks() / 1000.0;
}
