#include "gfx.h"
#include "random.h"

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

SDL_GLContext* sdlContext;

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

    SDL_Window *sdlWindow = SDL_CreateWindow(
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

    int imageUnits;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &imageUnits);
    printf("max texture image units %d\n", imageUnits);

    printWorkGroupLimits();

    return sdlWindow;
}

int compileShaders(struct Shaders *shaders)
{
    // everything is renderered onto the surface of one quad
    shaders->quad = compileQuadShaderProgram("bin/shader.vert", "bin/shader.frag");

    // preprass writes first-pass information into buffers for later acceleration 
    shaders->prepass = compileComputeShaderProgram("bin/rtpre.compute");

    // lighting traces multiple bounces of light around the scene 
    shaders->lighting = compileComputeShaderProgram("bin/rt.compute");

    // TODO: actually make sure all the shaders compile
    return 0;
}

GLuint compileQuadShaderProgram(const char* vsPath, const char* fsPath)
{
    GLuint vs = compileShader(vsPath, GL_VERTEX_SHADER);
    GLuint fs = compileShader(fsPath, GL_FRAGMENT_SHADER);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);

    // TODO: check program linking success textur
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

GLuint compileComputeShaderProgram(const char* computeShaderPath)
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

int createTextures(int width, int height, struct Shaders shaders, struct Textures* textures)
{
    // generate noise
    textures->noise = createNoiseTexture(width, height);
    glBindTexture(GL_TEXTURE_2D,textures->noise);
    int loc = glGetUniformLocation(shaders.lighting, "_noise");
    glUniform1i(loc, textures->noise);

    // create a texture for the compute shader to write to
    textures->target = createWriteOnlyTexture(width, height);

    textures->g0 = createTextureUnit(width, height, 2);
    textures->g1 = createTextureUnit(width, height, 3);

    return 0;
}

// creates a noise texture in active texture 1
GLuint createNoiseTexture(int width, int height)
{
    // same init steps as with a regular texture
    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_REPEAT);

    int channels = 4; // rgba
    int length = width*height*channels;
    printf("generating %d random floats\n", length);

    float* data = (float*)malloc(length*sizeof(float));

    for (int i = 0; i < length; i++)
    {
        data[i] = randomFloat();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, data);
    glBindImageTexture(0, texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);
    glGenerateMipmap(GL_TEXTURE_2D);

    free(data);

    return texture;
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

// creates an empty texture in GL_TEXTURE2 unit
GLuint createTextureUnit(int width, int height, int unit)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // an empty image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glBindImageTexture(unit, texture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);

    return texture;
}

void initBuffers()
{
    GLuint VAO, VBO, EBO;
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
