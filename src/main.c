#include "main.h"
#include "gfx.h"

const int WIDTH = 420;
const int HEIGHT = 420;

// forward declarations

// input
int checkQuit();

// time
float time();

void updateUniforms(GLuint shaderProgram);
void updateCameraUniforms(GLuint shaderProgram);

int main()
{
    // create a window and opengl context
    SDL_Window* window = gfxInit(WIDTH, HEIGHT);

    // create a texture for the compute shader to write to
    GLuint textureOutput = createWriteOnlyTexture(WIDTH, HEIGHT);
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
    float t = time();                                           // time
    float sin_t = sin(t);
    int tLocation = glGetUniformLocation(shaderProgram, "t");
    glUniform4f(tLocation, t, sin_t, (1.0 + sin_t)*0.5, 0.0f);

    updateCameraUniforms(shaderProgram);
}

void updateCameraUniforms(GLuint shaderProgram)
{
    // set up perspective projection matrix and its inverse
    mat4 proj, proji;
    float fovy = 60.0;                                          // vertical field of view in deg
    float near = 0.1;
    float far = 1000.0;
    float aspect = (float)WIDTH/(float)HEIGHT;
    glm_perspective(fovy,aspect,near,far,proj);                 // TODO: radians or degrees??
    glm_mat4_inv(proj, proji);
    int inverseProjLocation = glGetUniformLocation(shaderProgram, "_cameraInverseProjection");
    glUniformMatrix4fv(inverseProjLocation, 1, GL_FALSE, proji[0]);

    // form view space axes
    vec3 u,v;
    vec3 up = {0.0,1.0,0.0};
    vec3 w = {0.0,0.0,-1.0};
    glm_vec3_norm(w);
    glm_vec3_cross(up,w,u);
    glm_vec3_norm(u);
    glm_vec3_cross(w, u, v);
    int wLocation = glGetUniformLocation(shaderProgram, "_w");
    glUniform3f(wLocation+0, w[0], w[1], w[2]);                 // w
    glUniform3f(wLocation+1, u[0], u[1], u[2]);                 // u
    glUniform3f(wLocation+2, v[0], v[1], v[2]);                 // v

    // get camera properties
    float theta = glm_rad(fovy);                                    // convert fovy to radians
    float h = tan(theta*0.5);
    float vph = 2.0*h;                                          // viewport height
    float vpw = aspect*vph;                                     // viewport width

    // TODO: actual focal length lmao
    float f = 1.0;

    // camera axes
    //
    // use vec3_scale instead of vec3_multiply to get scalar multiplication
    // https://cglm.readthedocs.io/en/latest/vec3.html#c.glm_vec3_scale
    vec3 camh={0},
         camv={0},
         camll={0},
         camhh={0},
         camvh={0},
         fw={0};

    glm_vec3_scale(u,f*vpw,camh);   
    int camhLocation = glGetUniformLocation(shaderProgram, "_camh");
    glUniform3f(camhLocation, camh[0], camh[1], camh[2]);
    glm_vec3_scale(v,f*vph,camv);
    int camvLocation = camhLocation+1;
    glUniform3f(camvLocation, camv[0], camv[1], camv[2]);

    // camera lower left corner
    // calculate half-axes and w*f to establish 3d position of ll corner in camera space
    glm_vec3_scale(u,f*vpw*0.5,camhh);
    glm_vec3_scale(v,f*vph*0.5,camvh);
    glm_vec3_scale(w,f,fw);
    glm_vec3_sub(camll,camhh,camll);
    glm_vec3_sub(camll,camvh,camll);
    glm_vec3_sub(camll,fw,camll);
    int camllLocation = glGetUniformLocation(shaderProgram, "_camll");
    glUniform3f(camllLocation, camll[0], camll[1], camll[2]);
}

float time()
{
    // ms / 1000.0 = seconds since start
    return (float)SDL_GetTicks() / 1000.0f; 
}

int checkQuit()
{
    SDL_Event event;

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) return 1;

    return 0;
}

