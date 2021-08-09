#pragma once

#define GLEW_STATIC
#include "GL/glew.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "io.h"
#include "random.h"

SDL_Window* gfxInit(int width, int height);

unsigned int compileQuadShaderProgram(const char* vsPath, const char* fsPath);
unsigned int compileComputeShaderProgram();

GLuint createNoiseTexture(int width, int height);
void createTextureFromFile(const char* path);
GLuint createWriteOnlyTexture(int width, int height);
void printWorkGroupLimits();

void setVertexAttributes();
void initBuffers();

