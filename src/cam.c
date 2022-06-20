#include "cam.h"

const float FOVY = 90.0;

void updateCameraUniforms(GLuint shaderProgram, float aspect, float t)
{
    // wobble up dir
    vec3 cdir, cright, cup;
    vec3 up = {0.1*sin(t),1.0,0.05*cos(0.5*t)};
    glm_vec3_normalize(up);

    // camera and target pos
    float d = 10.0+sin(t);
    float pt = -t;
    vec3 cpos = {sin(pt)*d,3.0+cos(0.5*t)*2.5,cos(pt)*d};
    vec3 tpos = {0.0};
    int loc = glGetUniformLocation(shaderProgram, "_cpos");
    glUniform3fv(loc, 1, cpos);

    // form camera space axes
    glm_vec3_sub(cpos,tpos,cdir);
    glm_vec3_normalize(cdir);
    glm_vec3_cross(up,cdir,cright);
    glm_vec3_normalize(cright);
    glm_vec3_cross(cdir,cright,cup);
    glm_vec3_normalize(cup);
    loc = glGetUniformLocation(shaderProgram, "_w");
    glUniform3fv(loc, 1, cdir);
    loc = glGetUniformLocation(shaderProgram, "_u");
    glUniform3fv(loc, 1, cright);
    loc = glGetUniformLocation(shaderProgram, "_v");
    glUniform3fv(loc, 1, cup);

    // camera properties
    float f = 1.0;
    float theta = glm_rad(FOVY);
    float h = tan(theta*0.5);
    float vph = 2.0*h;
    float vpw = aspect*vph;

    // set up image plane axes
    vec3 camh={0},camv={0},camll={0},camhh={0},camvh={0},fw={0};

    glm_vec3_scale(cright,f*vpw,camh);
    loc = glGetUniformLocation(shaderProgram, "_camh");
    glUniform3fv(loc, 1, camh);
    glm_vec3_scale(cup,f*vph,camv);
    loc = glGetUniformLocation(shaderProgram, "_camv");
    glUniform3fv(loc, 1, camv);

    // camera lower left corner
    glm_vec3_scale(cright,f*vpw*0.5,camhh);
    glm_vec3_scale(cup,f*vph*0.5,camvh);
    glm_vec3_scale(cdir,f,fw);
    glm_vec3_sub(camll,camhh,camll);
    glm_vec3_sub(camll,camvh,camll);
    glm_vec3_sub(camll,fw,camll);
    loc = glGetUniformLocation(shaderProgram, "_camll");
    glUniform3fv(loc, 1, camll);
}
