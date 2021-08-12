#pragma once

#define GLEW_STATIC
#include "GL/glew.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "io.h"
#include "random.h"

struct Shaders
{
    GLuint quad;
    GLuint prepass;
    GLuint lighting;
};

struct Textures
{
    // texture that ultimately gets drawn to the framebuffer
    GLuint target;

    // TODO: blue noise, pink noise!
    // noise texture to help with randomness
    GLuint noise;

    // additional view space information
    //
    // x    depth
    // y    -
    // z    -
    // w    -
    GLuint g0;  
};

SDL_Window* gfxInit(int width, int height);

int compileShaders(struct Shaders *shaders);
GLuint compileQuadShaderProgram(const char* vsPath, const char* fsPath);
GLuint compileComputeShaderProgram(const char* csPath);

int createTextures(int width, int height, struct Shaders shaders, struct Textures* textures);
GLuint createNoiseTexture(int width, int height);
GLuint createTexture(int width, int height);
GLuint createWriteOnlyTexture(int width, int height);
GLuint compileShader(const char* path, GLenum type);

void printWorkGroupLimits();

// quad initialisation
void setVertexAttributes();
void initBuffers();

