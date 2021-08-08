#pragma once

#include <GL/glew.h>

#include <cglm/cam.h>

#include "clock.h"

void updateCameraUniforms(GLuint shaderProgram, float aspect);
