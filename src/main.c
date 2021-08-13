#include "main.h"
#include "gfx.h"
#include "clock.h"
#include "random.h"

#include "sphere.h"
#include "cam.h"
#include "input.h"

const int WIDTH = 420;
const int HEIGHT = 420;

void updateUniforms(GLuint shaderProgram);

SDL_Window *window;

struct Shaders shaders;
struct Textures textures;

void initialise()
{
    window = gfxInit(WIDTH, HEIGHT);

    randomInit();

    // initialise quad
    initBuffers();
    setVertexAttributes();

    // compile shader programs
    compileShaders(&shaders);
    createTextures(WIDTH, HEIGHT, shaders, &textures);
}

int main()
{
    initialise();

    float start = now();
    int frames;
    for (frames = 0; !checkQuit(); frames++)
    {
        GLuint shader;

        // prepass
        // TODO: write output to different texture than main output
        shader = shaders.prepass;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures.g0);
        glUseProgram(shader);
        updateUniforms(shader);
        glDispatchCompute((GLuint)WIDTH, (GLuint)HEIGHT, 1);

        // make sure we're finished writing to the texture before trying to read it
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // dispatch compute shaders
        shader = shaders.lighting;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures.target);
        glUseProgram(shader);
        updateUniforms(shader);
        //int loc = glGetUniformLocation(shader, "_g0");
        //glUniform1i(loc, textures.g0);
        glDispatchCompute((GLuint)WIDTH, (GLuint)HEIGHT, 1);

        // make sure we're finished writing to the texture before trying to read it
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // normal drawing pass
        shader = shaders.quad;
        glUseProgram(shader);

        // bind texture written to by compute stage to 2d target
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // swip swap
        SDL_GL_SwapWindow(window);
    }

    float elapsed = now()-start;
    printf("%d frames in %fs [%f fps]\n", 
            frames, 
            elapsed, 
            (float)frames/elapsed);

    return 0;
}

void updateUniforms(GLuint shaderProgram)
{
    // update random values
    vec4 seed = 
    {
        randomFloat(),
        randomFloat(),
        randomFloat(),
        randomFloat()
    };
    int loc = glGetUniformLocation(shaderProgram, "_seed");
    glUniform4fv(loc, 1, seed);

    // update time
    float t = now();
    float sin_t = sin(t);
    loc = glGetUniformLocation(shaderProgram, "_t");
    glUniform4f(loc, t, sin_t, (1.0 + sin_t)*0.5, 0.0f);

    // update camera
    float aspect = (float)WIDTH/(float)HEIGHT;
    updateCameraUniforms(shaderProgram, aspect);

    // make and update spheres
    const int sphereCount = 42;
    struct Sphere spheres[sphereCount];
    makeSpheres(spheres, sphereCount);
    updateSphereUniforms(shaderProgram, spheres, sphereCount);
}
