#version 430 core
layout (location = 0) in vec3 aPos;         // position has attribute position 0
layout (location = 1) in vec3 aColor;       // color has attribute position 1
layout (location = 2) in vec2 aTexCoord;    // texture coordinate

out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}
