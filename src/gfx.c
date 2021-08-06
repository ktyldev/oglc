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

SDL_Window* sdlWindow;
SDL_GLContext* sdlContext;

GLuint compileShader(const char* path, GLenum type);

SDL_Window* gfxInit(int width, int height)
{
    // load sdl modules
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
    {
        printf("%s\n", "unable to set video mode");
        exit(1);
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    sdlWindow = SDL_CreateWindow(
            "oglc",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width,
            height,
            SDL_WINDOW_OPENGL);

    sdlContext = SDL_GL_CreateContext(sdlWindow);

    glewExperimental = GL_TRUE;
    glewInit();

    // intiliased opengl

    printf("%s\n", glGetString(GL_RENDERER));

    int availableAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &availableAttributes);
    printf("max vertex attributes %d\n", availableAttributes);

    int uniformLocations;
    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &uniformLocations);
    printf("max uniform locations %d\n", uniformLocations);

    return sdlWindow;
}

unsigned int compileQuadShaderProgram(const char* vsPath, const char* fsPath)
{
    GLuint vs = compileShader(vsPath, GL_VERTEX_SHADER);
    GLuint fs = compileShader(fsPath, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);

    // TODO: check program linking success
    glLinkProgram(shaderProgram);

    GLint result = GL_FALSE;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &result);
    if (result == GL_FALSE)
    {
        GLint logLength = 0;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &logLength);

        char err[logLength];
        glGetProgramInfoLog(shaderProgram, logLength, NULL, err);

        fputs("error linking quad shader program:\n", stderr);
        fputs(err, stderr); 

        exit(1);
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return shaderProgram;
}

unsigned int compileComputeShaderProgram(const char* computeShaderPath)
{
    GLuint cs = compileShader(computeShaderPath, GL_COMPUTE_SHADER);
    GLuint computeProgram = glCreateProgram(); 
    glAttachShader(computeProgram, cs);

    glLinkProgram(computeProgram);

    glDeleteShader(cs);

    return computeProgram;
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

    return shader;
}

void setVertexAttributes()
{
    int stride = 8*sizeof(float);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    // uv 
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);
}

void createTextureFromFile(const char* imagePath)
{
    // create an opengl texture and bind it to the GL_TEXTURE_2D target
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);

    // load image data from file
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
    if (!data)
    {
        fputs("failed to load texture data: ", stderr);
        fputs(imagePath, stderr);
        fputs("\n", stderr);
        exit(1);
    }

    // read image data into bound opengl texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

GLuint createWriteOnlyTexture(int width, int height)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    return texture;
}

void initBuffers()
{
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO); // vertex array object
    glGenBuffers(1, &VBO);      // vertex buffer object
    glGenBuffers(1, &EBO);      // element buffer object

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void printWorkGroupLimits()
{
    int workGroupCount[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &workGroupCount[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &workGroupCount[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &workGroupCount[2]);
    printf(
            "max work group counts x:%i y:%i z:%i\n", 
            workGroupCount[0], 
            workGroupCount[1], 
            workGroupCount[2]);

    int workGroupSize[3];
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &workGroupSize[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &workGroupSize[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &workGroupSize[2]);
    printf(
            "max work group sizes x:%i y:%i z:%i\n", 
            workGroupSize[0], 
            workGroupSize[1], 
            workGroupSize[2]);

    int workGroupInvocations;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &workGroupInvocations);
    printf("max work group invocations %i\n", workGroupInvocations);
}
