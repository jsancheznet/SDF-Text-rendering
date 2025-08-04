#include <fstream>
#include <iostream>

#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"
#include "camera.h"
#include "mouse.h"
#include "typedefs.h"

// TODO LIST:
//     - Ir a parte donde se hace el pannning de camara, Hacer una lista de papel de las cosas que voy a implementar
//     - Centrar el texto
//     - Hacer que el texto pequenio se vea mejor,
//     - Batchear los caracteres en solo una draw call
//     - Hacer un refactor quitando la variable QuadPositionX de globales
//     - Implementar Outlines https://www.redblobgames.com/blog/2024-08-27-sdf-font-outlines/
//     - Escribir con texto chico a la izquierda arriba de las ventanas las teclas y que hace cada una
//     - Agregar kerning

b32 IsRunning = true;

f32 QuadPositionX = 0.0f;

renderer Renderer = {};
camera Camera = {};
mouse Mouse = {};

void ProcessEvents()
{
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
        switch (Event.type)
        {
            case SDL_EVENT_QUIT:
            {
                IsRunning = false;
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                if (Event.key.key == SDLK_ESCAPE)
                {
                    IsRunning = false;
                }
                if(Event.key.key == SDLK_RIGHT)
                {
                    QuadPositionX += 0.2f;
                }
                if(Event.key.key == SDLK_LEFT)
                {
                    QuadPositionX -= 0.2f;
                }
                break;
            }

            case SDL_EVENT_KEY_UP:
            {
                break;
            }

            case SDL_EVENT_WINDOW_RESIZED:
            {
                i32 Width = Event.window.data1;
                i32 Height = Event.window.data2;
                glViewport(0, 0, Width, Height);
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL:
            {
                Camera.ApplyZoom(Event.wheel.y);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
            case SDL_EVENT_MOUSE_MOTION:
            default:
            {
                break;
            }
        }
    }
}

int main(i32 Argc, char** Argv)
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_FRAMEBUFFER_SRGB_CAPABLE, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    // TODO(Jsanchez): Turn debug off on release
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    SDL_Window* Window = SDL_CreateWindow("SDF Text Rendering", 1366, 768, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

    Renderer.Init(Window);
    Mouse.Init();

    // Camera Configuration
    Camera.Position = glm::vec3(0.0f, 0.0f, 3.0f);
    Camera.Target = glm::vec3(0.0f, 0.0f, 0.0f);
    Camera.Up = glm::vec3(0.0f, 1.0f, 0.0f);
    Camera.View = glm::lookAt(Camera.Position, Camera.Target, Camera.Up);

    Camera.Width = 240.0f;
    Camera.Height = 135.0f;
    Camera.Near = 0.1f;
    Camera.Far = 100.0f;
    Camera.Projection = glm::ortho(-Camera.Width, Camera.Width, -Camera.Height, Camera.Height, Camera.Near, Camera.Far);

    while (IsRunning)
    {
        ProcessEvents();

        Mouse.UpdateState();

        if(Mouse.IsPressed(RIGHT_BUTTON))
        {
            f32 DeltaX = Mouse.RelativePosX * 0.2f;
            f32 DeltaY = Mouse.RelativePosY * 0.2f;
            Camera.Position.x -= DeltaX;
            Camera.Target.x -= DeltaX;
            Camera.Position.y += DeltaY;
            Camera.Target.y += DeltaY;
            Camera.View = glm::lookAt(Camera.Position, Camera.Target, Camera.Up);

            Mouse.SetCursor(Mouse.GrabbingCursor);
        }
        else
        {
            Mouse.SetCursor(Mouse.DefaultCursor);
        }

        Renderer.BeginFrame();

        Renderer.UpdateCamera(&Camera);

        // Update and set rendering variables to quad!
        float Scale = 40.0f;
        glm::vec3 QuadScale = glm::vec3(Scale, Scale, Scale);
        glm::vec3 QuadPosition = {QuadPositionX, 0.0f, 0.0f};
        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::scale(Model, QuadScale);
        Model = glm::translate(Model, QuadPosition);
        i32 ModelLocation = glGetUniformLocation(Renderer.ExampleShader, "Model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(Model));

        Renderer.RenderText("abcdefghijklmnñopqrstuvwxyz");

        Renderer.EndFrame();
    }

    SDL_Quit();
}
