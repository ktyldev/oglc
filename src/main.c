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
            "bin/res/rt.compute");
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
        float t = time();                                           // time
        float sin_t = sin(t);
        int tLocation = glGetUniformLocation(computeProgram, "t");
        glUniform4f(tLocation, t, sin_t, (1.0 + sin_t)*0.5, 0.0f);

        // form view space axes
        vec3 u,v;
        vec3 up = {0,1.0,0};
        vec3 w = {0,sin_t*0.1,-1.0};
        glm_vec3_norm(w);
        glm_vec3_cross(up,w,u);
        glm_vec3_norm(u);
        glm_vec3_cross(w, u, v);
        int wLocation = glGetUniformLocation(computeProgram, "w");
        glUniform3f(wLocation+0, w[0], w[1], w[2]);                 // w
        glUniform3f(wLocation+1, u[0], u[1], u[2]);                 // u
        glUniform3f(wLocation+2, v[0], v[1], v[2]);                 // v

        // dispatch compute shader
        glDispatchCompute((GLuint)width, (GLuint)height, 1);

        // make sure we're finished writing to the texture before trying to read it
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // normal drawing pass
        glUseProgram(quadProgram);
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

