#pragma once

#include <SDL2/SDL.h>

struct Epoch
{
    float speed;
};

// seconds since program start
float now(struct Epoch e);
