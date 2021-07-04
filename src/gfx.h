#pragma once

#define GLEW_STATIC
#include "GL/glew.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "io.h"

struct GraphicsContext
{
    SDL_Window*     window;
    SDL_GLContext*  context;
};

SDL_Window* createWindow();
struct GraphicsContext createContext();
GLuint compileShader(const char* path, GLenum type);

