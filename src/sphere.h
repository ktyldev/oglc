#pragma once

#include <stdio.h>

#define GLEW_STATIC
#include <GL/glew.h>

#include <cglm/vec3.h>

#include "time.h"

struct Sphere
{
    vec4 cr;
    vec3 albedo;
};

void makeSpheres(struct Sphere *spheres, int count);
struct Sphere makeSphere(vec3 center, float radius, vec3 albedo);
void updateSphereUniform(GLuint shaderProgram, struct Sphere sphere);
void updateSphereUniforms(GLuint shaderProgram, struct Sphere *spheres, int count);
