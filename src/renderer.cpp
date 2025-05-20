#include "renderer.h"

#include <cstdio>
#include <iostream>

#include <SDL3/SDL.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Renderer::Renderer(SDL_Window* Window) : Window(Window)
{
}

void Renderer::Init()
{
    gladLoadGL();

    { // Enable OpenGL Debug Mode
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    { // Print extensions list to stdout
        // Get the number of extensions
        GLint numExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

        // Print all extensions
        for (GLint i = 0; i < numExtensions; i++) {
            const char* extensionName = (const char*)glGetStringi(GL_EXTENSIONS, i);
            std::cout << "Extension #" << i << ": " << extensionName << std::endl;
        }
    }

    ExampleShader = CompileShader("shaders/hello.glsl");
    ExampleTexture = CreateTexture("assets/Arial.png");

    f32 Vertices[] =
    {
        // Positions        // Texture Coordinates
        0.5f, 0.5f, 0.0f, 0.5200, 0.3370f, // top right
        0.5f, -0.5f, 0.0f, 0.5200f, 0.2120f, // bottom right
        -0.5f, -0.5f, 0.0f, 0.4040f, 0.2120f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.4040f, 0.3370f // top left
    };

    u32 Indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    glCreateVertexArrays(1,&VAO);

    glCreateBuffers(1,  &VBO);
    glNamedBufferStorage(VBO, sizeof(Vertices), Vertices, GL_DYNAMIC_STORAGE_BIT);

    glCreateBuffers(1, &EBO);
    glNamedBufferStorage(EBO, sizeof(Indices), Indices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(f32) * 5); // f32 *5 is the stride between each vertex
    glVertexArrayElementBuffer(VAO, EBO);

    glEnableVertexArrayAttrib(VAO, 0); // Positions
    glEnableVertexArrayAttrib(VAO, 1); // Texture Coordinates

    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0); // Specify the attrib format for the positions
    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, sizeof(f32) * 3); // Specify the attrib format for the texture coordinates

    glVertexArrayAttribBinding(VAO, 0, 0); // I don't understand the last parameter, WTF? Why always 0?
    glVertexArrayAttribBinding(VAO, 1, 0);

    glBindTextureUnit(0, ExampleTexture);
}

void Renderer::BeginFrame()
{
    glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);

    glUseProgram(ExampleShader);
}

void Renderer::EndFrame()
{
    SDL_GL_SwapWindow(Window);
}

u32 Renderer::CompileShader(const char* Filename)
{
    Assert(Filename);

    size_t Size;
    char *FileString = static_cast<char*>(SDL_LoadFile(Filename, &Size));

    u32 VertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *VertexSource[2] = {"#version 460 core\n#define VERTEX_SHADER\n", FileString};
    glShaderSource(VertexShader, 2, VertexSource, NULL);
    glCompileShader(VertexShader);
    i32 Compiled;
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(VertexShader, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        VertexShader = 0;
    }


    u32 FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *FragmentSource[2] = {"#version 460 core\n#define FRAGMENT_SHADER\n", FileString};
    glShaderSource(FragmentShader, 2, FragmentSource, NULL);
    glCompileShader(FragmentShader);
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(FragmentShader, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        FragmentShader = 0;
    }

    u32 CompiledShader = glCreateProgram();
    glAttachShader(CompiledShader, VertexShader);
    glAttachShader(CompiledShader, FragmentShader);
    glLinkProgram(CompiledShader);
    i32 IsLinked = 0;
    glGetProgramiv(CompiledShader, GL_LINK_STATUS, (GLint *)&IsLinked);
    if (IsLinked == GL_FALSE)
    {
        i32 MaxLogLength = 1024;
        char InfoLog[1024] = {0};
        glGetProgramInfoLog(CompiledShader, MaxLogLength, &MaxLogLength, &InfoLog[0]);
        printf("%s: SHADER PROGRAM FAILED TO COMPILE/LINK\n", Filename);
        printf("%s\n", InfoLog);
        glDeleteProgram(CompiledShader);
        CompiledShader = 0;
    }

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    SDL_free(FileString);

    return CompiledShader;
}

u32 Renderer::CreateTexture(const char* Filepath)
{
    u32 Handle;

    i32 Width, Height, ChannelCount;
    stbi_set_flip_vertically_on_load(true);
    u8 *ImageData = stbi_load(Filepath, &Width, &Height, &ChannelCount, 4);

    glCreateTextures(GL_TEXTURE_2D, 1, &Handle);

    glTextureParameteri(Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(Handle, 1, GL_RGBA8, Width, Height);

    glTextureSubImage2D(Handle, 0, 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);

    glGenerateTextureMipmap(Handle);

    stbi_image_free(ImageData);

    return Handle;
}

void Renderer::OpenGLDebugMessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, GLchar const* Message, void const* UserParam)
{
    using namespace std;

    Source;
    Type;
    Id;
    Severity;
    Length;
    Message;
    UserParam;

    switch (Source)
    {
        case GL_DEBUG_SOURCE_API: cout << "API:"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: cout << "WINDOW SYSTEM:"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: cout << "SHADER COMPILER:"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: cout << "THIRD PARTY:"; break;
        case GL_DEBUG_SOURCE_APPLICATION: cout << "APPLICATION:"; break;
        case GL_DEBUG_SOURCE_OTHER: cout << "OTHER:"; break;
    }

    switch (Type)
    {
        case GL_DEBUG_TYPE_ERROR: cout << "ERROR:"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: cout << "DEPRECATED_BEHAVIOR:"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: cout << "UNDEFINED_BEHAVIOR:"; break;
        case GL_DEBUG_TYPE_PORTABILITY: cout << "PORTABILITY:"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: cout << "PERFORMANCE:"; break;
        case GL_DEBUG_TYPE_MARKER: cout << "MARKER:"; break;
        case GL_DEBUG_TYPE_OTHER: cout << "OTHER:"; break;
    }

    switch (Severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION: cout << "NOTIFICATION:"; break;
        case GL_DEBUG_SEVERITY_LOW: cout << "LOW:"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: cout << "MEDIUM:"; break;
        case GL_DEBUG_SEVERITY_HIGH: cout << "HIGH:"; break;
    }

    cout << "->  " << Message << endl;
}
