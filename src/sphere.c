#include "sphere.h"

void makeSpheres(struct Sphere *spheres, int count)
{
    //float t = time();

    vec3 albedos[] = 
    {
        {0.0,0.0,1.0},
        {0.0,1.0,0.0},
        {0.0,1.0,1.0},
        {1.0,0.0,0.0},
        {1.0,0.0,1.0},
        {1.0,1.0,0.0},
        {1.0,1.0,1.0}
    };

    // distance from center
    float d = 6.0;
    float radius = 0.5;
    float x;
    vec3 sc = {0.0,0.0,1.0};

    float t = now();
    for (int i = 0; i < count; i++)
    {
        x = 2.0*CGLM_PI * (float)i/(float)count;
        sc[0] = sin(x)*d;
        sc[1] = sin(x*3.0-t);
        sc[2] = cos(x)*d;

        float ic = i/(float)count*CGLM_PI*2.0;
        float r = sin(ic);
        float g = sin(ic+CGLM_PI/1.5);
        float b = sin(ic+2.0*CGLM_PI/1.5);

        vec3 col = {r,g,b};
        glm_vec3_scale(col, 0.5, col);
        glm_vec3_adds(col, 0.5, col);

        spheres[i] = makeSphere(sc,radius,col);
    }
}

struct Sphere makeSphere(vec3 center, float radius, vec3 albedo)
{
    struct Sphere s;

    s.cr[0] = center[0];
    s.cr[1] = center[1];
    s.cr[2] = center[2];
    s.cr[3] = radius;

    glm_vec3_copy(albedo, s.albedo);

    return s;
}

void updateSphereUniform(GLuint shaderProgram, struct Sphere sphere)
{
    int scrloc, saloc;
    scrloc = glGetUniformLocation(shaderProgram, "_sphere.cr");
    saloc = glGetUniformLocation(shaderProgram, "_sphere.albedo");

    glUniform4fv(scrloc, 1, sphere.cr);
    glUniform3fv(saloc, 1, sphere.albedo);

    updateSphereUniforms(shaderProgram, &sphere, 0);
}

void updateSphereUniforms(GLuint shaderProgram, struct Sphere *spheres, int count)
{
    // set sphere count 
    int loc = glGetUniformLocation(shaderProgram, "_activeSpheres");
    glUniform1i(loc, count);

    // each sphere takes up two uniform locations
    const int stride = 2;         

    // first location in the array
    loc = glGetUniformLocation(shaderProgram, "_spheres[0].cr"); 

    for (int i = 0; i < count; i++)
    {
        struct Sphere s = spheres[i];
        glUniform4fv(loc+i*stride,   1,  s.cr);
        glUniform3fv(loc+i*stride+1, 1,  s.albedo);
    }
}
