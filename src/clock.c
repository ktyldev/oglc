#include "clock.h"

float now() 
{ 
    return (float)SDL_GetTicks() / 1000.0; 
}
