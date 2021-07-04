#include "gfx.h"

SDL_Window* createWindow()
{
    // load sdl modules
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("%s\n", "unable to set video mode");
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    SDL_Window* window = SDL_CreateWindow(
            "oglc",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            800,
            600,
            SDL_WINDOW_OPENGL);

    return window;
}

struct GraphicsContext createContext()
{
    SDL_Window* window = createWindow();

    SDL_GLContext context = SDL_GL_CreateContext(window);

    glewExperimental = GL_TRUE;
    glewInit();

    struct GraphicsContext ctx = {window, context};

    return ctx;
}

GLuint compileShader(const char* path, GLenum type)
{
    // read shader file into buffer
    char* buffer;
    buffer = calloc(1, getFileSize(path)+1); // calloc -> contiguous allocation
    readFile(path, buffer);

    GLuint shader = glCreateShader(type);
    GLint result = GL_FALSE;
    int logLength;

    // compile
    const char* src = buffer;               // glShaderSource expects a const char*

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    free(buffer);

    // check
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    char err[logLength];
    glGetShaderInfoLog(shader, logLength, NULL, err);
    if (logLength > 1) 
    {
        fputs("error compiling shader ", stderr);
        fputs(path, stderr); 
        fputs(":\n", stderr);
        fputs(err, stderr); 

        exit(1);
    }

    printf(":: . compiled shader %s! :D\n", path);

    return shader;
}
