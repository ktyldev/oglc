#include "sphere.h"

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
