#pragma once

#include <SDL2/SDL.h>

struct Epoch
{
    float scale;
};

// real seconds since program start
float now();
// simulation seconds since program start
float nowScaled(struct Epoch e);
