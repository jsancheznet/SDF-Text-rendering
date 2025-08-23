#pragma once

#include "typedefs.h"
#include <glad/glad.h>

#include "SDL3/SDL_video.h"

#include "bitmap_font.h"
#include "camera.h"

struct alignas(16) camera_uniform_data
{
    glm::mat4 View;
    glm::mat4 Projection;
};

struct renderer
{
    u32 VAO = 0;
    u32 VBO = 0;
    u32 EBO = 0;
    u32 CameraUBO;

    u32 ExampleShader = 0;

    void Init(SDL_Window *WindowIn);
    void BeginFrame();
    void EndFrame();
    u32 CompileShader(const char* Filename);
    u32 CreateTexture(const char* Filepath);
    void UpdateCamera(camera *Camera);

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
