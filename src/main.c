#include "main.h"

#include "gfx.h"

// data !
const char* vertShaderPath = "res/shader/shader.vert";
const char* fragShaderPath = "res/shader/shader.frag";

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

// forward declarations
int checkQuit();


int main()
{
    // initialise opengl graphics context
    const struct GraphicsContext ctx = createContext();

    // compile shaders
    printf(":: compiling shaders\n");
    GLuint vertShader = compileShader(vertShaderPath, GL_VERTEX_SHADER);
    GLuint fragShader = compileShader(fragShaderPath, GL_FRAGMENT_SHADER);

    printf(":: linking shader program\n");
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    // TODO: check program linking success

    // set up vertex array object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // set up vertex buffer object
    unsigned int VBO;                                                   
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    // copy our vertices into a buffer opengl can use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);                                         // bind it to the GL_ARRAY_BUFFER target
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);  // copy user-defined data into currently bound buffer

    // set vertex attributes
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0); // TODO: wtf
    glEnableVertexAttribArray(0);

    // wait for exit
    while (!checkQuit())
    {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        SDL_GL_SwapWindow(ctx.window);
    }

    return 0;
}

int checkQuit()
{
    SDL_Event event;

    if (SDL_PollEvent(&event) && event.type == SDL_QUIT) return 1;

    return 0;
}

