#pragma once

#define GLEW_STATIC
#include "GL/glew.h"


#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "io.h"

SDL_Window* gfxInit();

unsigned int compileShaderProgram();

