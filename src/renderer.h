#pragma once

#include "typedefs.h"
#include <glad/glad.h>

#include "SDL3/SDL_video.h"
#include "bitmap_font.h"

struct Renderer
{

    Renderer(SDL_Window *Window);

    u32 VAO = 0;
    u32 VBO = 0;
    u32 EBO = 0;

    u32 ExampleShader = 0;

    void Init();
    void BeginFrame();
    void EndFrame();
    u32 CompileShader(const char* Filename);
    u32 CreateTexture(const char* Filepath);

    // Font And Text Drawing
    bitmap_font DefaultFont;

    bitmap_font LoadFont(std::string Json, std::string Image);

    void RenderText(std::string Text);

    // TODO(Jsanchez): DrawSprite(Sprite), on this function just maintain a batch renderer!, we can use the opengl bible
    // https://jasonliang.js.org/batch-renderer.html, this looks like a good read

private:

    SDL_Window* Window = nullptr;

    static void OpenGLDebugMessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, GLchar const* Message, void const* UserParam);
};
