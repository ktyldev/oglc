#include "main.h"

#include "gfx.h"

// forward declarations

// input
int checkQuit();

// time
float time();

int main()
{
    SDL_Window* window = gfxInit();

    createTextureFromFile();

    initBuffers();

    setVertexAttributes();

    unsigned int shaderProgram = compileShaderProgram();
    glUseProgram(shaderProgram);

    // render loop
    while (!checkQuit())
    {
        // clear background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

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

