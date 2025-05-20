#pragma once

#include "typedefs.h"
#include <glad/glad.h>

#include "SDL3/SDL_video.h"

class Renderer
{

public:

    u32 VAO = 0;
    u32 VBO = 0;
    u32 EBO = 0;

    u32 ExampleShader = 0;
    u32 ExampleTexture = 0;

    Renderer(SDL_Window *Window);

    void Init();

    void BeginFrame();
    void EndFrame();

    u32 CompileShader(const char* Filename);
    u32 CreateTexture(const char* Filepath);

    // TODO(Jsanchez): DrawSprite(Sprite), on this function just maintain a batch renderer!, we can use the opengl bible
    // https://jasonliang.js.org/batch-renderer.html, this looks like a good read

private:

    SDL_Window* Window = nullptr;

    static void OpenGLDebugMessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, GLchar const* Message, void const* UserParam);
};
