#pragma once

#define GLEW_STATIC
#include "GL/glew.h"


#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "io.h"

void gfxInit();
SDL_Window* getWindow();
SDL_GLContext* getContext();

unsigned int compileShaderProgram();

