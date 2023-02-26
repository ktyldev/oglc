#include "main.h"
#include "gfx.h"
#include "clock.h"
#include "random.h"

#include "sphere.h"
#include "cam.h"
#include "input.h"

const int WIDTH = 500;
const int HEIGHT = 500;

void updateUniforms(GLuint shaderProgram, float t);

SDL_Window *window;

struct Shaders shaders;
struct Textures textures;
struct Epoch epoch;

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

void parseArgs(int argc, char* argv[], struct Epoch* e)
{
    // check we have the right number of arguments
    if (argc != 2)
    {
        fprintf(stderr, "usage: oglc TIMESPEED\n");
    }

    sscanf(argv[1], "%f", &(e->scale));
}

int main(int argc, char* argv[])
{
    parseArgs(argc, argv, &epoch);

    initialise();

    float start = now();
    int frames;
    for (frames = 0; !checkQuit(); frames++)
    {
        GLuint shader;
        float t = nowScaled(epoch);

        // prepass
        // TODO: write output to different texture than main output
        shader = shaders.prepass;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures.g0);
        glUseProgram(shader);
        updateUniforms(shader, t);
        glDispatchCompute((GLuint)WIDTH, (GLuint)HEIGHT, 1);

        // make sure we're finished writing to the texture before trying to read it
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        shader = shaders.lighting;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures.target);
        glUseProgram(shader);
        updateUniforms(shader, t);
        //int loc = glGetUniformLocation(shader, "_g0");
        //glUniform1i(loc, textures.g0);
        glDispatchCompute((GLuint)WIDTH, (GLuint)HEIGHT, 1);

        // make sure we're finished writing to the texture before trying to read it
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // TODO: frame blending
        //
        // before we get to our normal rendering pass we need to blend our samples
        // across multiple frames. to do this, we should maintain an accumulation
        // buffer where we can store pixel data while a new frame is being generated.
        // this can be a collection of two buffers, where one is written to and the 
        // other is read from. to render a new frame, the newly sampled raw frame can
        // be combined with data from the previous frame. by repeatedly taking the
        // average over a large number of frames a cleaner image can be generated.
        // this is most effective for static images, and will produce motion blur on
        // a moving image.

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

void updateUniforms(GLuint shaderProgram, float t)
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

    float sin_t = sin(t);
    loc = glGetUniformLocation(shaderProgram, "_t");
    glUniform4f(loc, t, sin_t, (1.0 + sin_t)*0.5, 0.0f);

    // update camera
    float aspect = (float)WIDTH/(float)HEIGHT;
    updateCameraUniforms(shaderProgram, aspect, t);

    // make and update spheres
    const int sphereCount = 25;
    struct Sphere spheres[sphereCount];
    makeSpheres(spheres, sphereCount, t);
    updateSphereUniforms(shaderProgram, spheres, sphereCount);
}
