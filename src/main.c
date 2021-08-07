#include "main.h"
#include "gfx.h"
#include "time.h"

#include "sphere.h"
#include "cam.h"

const int WIDTH = 420;
const int HEIGHT = 420;

void updateUniforms(GLuint shaderProgram);

int main()
{
    // create a window and opengl context
    SDL_Window* window = gfxInit(WIDTH, HEIGHT);

    // create a texture for the compute shader to write to
    GLuint textureOutput = createWriteOnlyTexture(WIDTH, HEIGHT);
    printWorkGroupLimits();

    // compile shader programs
    unsigned int computeProgram = compileComputeShaderProgram(
            "bin/rt.compute");
    unsigned int quadProgram = compileQuadShaderProgram(
            "bin/shader.vert", 
            "bin/shader.frag");

    // initialise quad
    initBuffers();
    setVertexAttributes();

    // render loop
    while (!checkQuit())
    {
        glUseProgram(computeProgram);

        updateUniforms(computeProgram);

        // dispatch compute shader
        glDispatchCompute((GLuint)WIDTH, (GLuint)HEIGHT, 1);

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

void updateUniforms(GLuint shaderProgram)
{
    float t = now();
    float sin_t = sin(t);
    int tLocation = glGetUniformLocation(shaderProgram, "_t");
    glUniform4f(tLocation, t, sin_t, (1.0 + sin_t)*0.5, 0.0f);

    float aspect = (float)WIDTH/(float)HEIGHT;
    updateCameraUniforms(shaderProgram, aspect);

    const int sphereCount = 42;
    struct Sphere spheres[sphereCount];
    makeSpheres(spheres, sphereCount);

    updateSphereUniforms(shaderProgram, spheres, sphereCount);
}
