#include "main.h"
#include "gfx.h"

// forward declarations

// input
int checkQuit();

// time
float time();

int main()
{
    int width = 420;
    int height = 420;

    // create a window and opengl context
    SDL_Window* window = gfxInit(width, height);

    // create a texture for the compute shader to write to
    GLuint textureOutput = createWriteOnlyTexture(width, height);
    printWorkGroupLimits();

    // compile shader programs
    unsigned int computeProgram = compileComputeShaderProgram(
            "bin/res/compute.compute");
    unsigned int quadProgram = compileQuadShaderProgram(
            "bin/res/shader.vert", 
            "bin/res/shader.frag");

    // initialise quad
    initBuffers();
    setVertexAttributes();

    // render loop
    while (!checkQuit())
    {
        glUseProgram(computeProgram);

        // update uniforms
        float t = time();
        float sin_t = sin(t);
        int tLocation = glGetUniformLocation(computeProgram, "t");
        glUniform4f(tLocation, t, (1.0 + sin_t)*0.5, 0.0f, 0.0f);

        // dispatch compute shader
        glDispatchCompute((GLuint)width, (GLuint)height, 1);

        // make sure we're finished writing to the texture before trying to read it
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // normal drawing pass
        glUseProgram(quadProgram);
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glActiveTexture(GL_TEXTURE0);                                           // use computed texture
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

