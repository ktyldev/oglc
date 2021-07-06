#include "main.h"

#include "gfx.h"
#include "stb_image.h"

float vertices[] = {
    // position             color               uvs
     1.0f,  1.0f,  0.0f,    1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
     1.0f, -1.0f,  0.0f,    0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
    -1.0f, -1.0f,  0.0f,    0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
    -1.0f,  1.0f,  0.0f,    1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
};

unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3
};

// forward declarations

// input
int checkQuit();

// time
float time();

int main()
{
    gfxInit();
    SDL_Window* window = getWindow();

    // generate opengl texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image data from file
    const char* imageFile = "res/tex.png";
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(imageFile, &width, &height, &nrChannels, 0);
    if (!data)
    {
        fputs("failed to load texture data: ", stderr);
        fputs(imageFile, stderr);
        fputs("\n", stderr);
        exit(1);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    // free loaded image data
    stbi_image_free(data);

    unsigned int shaderProgram = compileShaderProgram();

    // vertex array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    // vertex buffer object
    unsigned int VBO;                                                   
    glGenBuffers(1, &VBO);
    // element buffer object
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // copy our vertices into a buffer opengl can use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                         // bind it to the GL_ARRAY_BUFFER target
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // copy user-defined data into currently bound buffer

    // copy indices into a buffer for opengl
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // set vertex attributes

    int stride = 8*sizeof(float);
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); // TODO: wtf
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glUseProgram(shaderProgram);

    // render loop
    while (!checkQuit())
    {
        // clear background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // update uniforms
        //float time = (float)SDL_GetTicks() / 1000.0f;
        //float greenValue = (sin(time)/2.0f)+0.5f;
        //int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        //glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

        // draw
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        SDL_GL_SwapWindow(window);
    }

    return 0;
}

float time()
{
    return (float)SDL_GetTicks() / 1000.0f; // ms / 1000.0 = seconds since start
}

int checkQuit()
{
    SDL_Event event;

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) return 1;

    return 0;
}

