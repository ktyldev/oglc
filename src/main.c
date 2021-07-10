#include "main.h"

#include "gfx.h"

// forward declarations

// input
int checkQuit();

// time
float time();

int main()
{
    int width = 800;
    int height = 600;
    const char* texPath = "res/tex.png";

    // create a window and opengl context
    SDL_Window* window = gfxInit(width, height);

    // create a texture for the compute shader to write to
    GLuint textureOutput = createWriteOnlyTexture(width, height);
    printWorkGroupLimits();

    // compile shader programs
    unsigned int computeProgram = compileComputeShaderProgram("res/shader/compute.glsl");
    unsigned int quadProgram = compileQuadShaderProgram(
            "res/shader/shader.vert", 
            "res/shader/shader.frag");

    // initialise quad
    initBuffers();
    setVertexAttributes();

    // render loop
    while (!checkQuit())
    {
        glUseProgram(computeProgram);

        // update uniforms
        float t = time();
        t = (sin(t)/2.0f)+0.5f;
        int tLocation = glGetUniformLocation(computeProgram, "t");
        glUniform1f(tLocation, t);

        // dispatch compute shader
        glDispatchCompute((GLuint)width, (GLuint)height, 1);

        // make sure we're finished writing to the texture before trying to read it
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // normal drawing pass
        glUseProgram(quadProgram);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);                   // use computed texture
        glBindTexture(GL_TEXTURE_2D, textureOutput);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // swip swap
        SDL_GL_SwapWindow(window);
    }

    return 0;
}

float time()
{
    return (float)SDL_GetTicks() / 1000.0f; // ms / 1000.0 = seconds since start
}

int checkQuit()
{
    SDL_Event event;

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) return 1;

    return 0;
}

