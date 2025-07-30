#include <fstream>
#include <iostream>

#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"
#include "camera.h"
#include "typedefs.h"

// TODO LIST:
//     - Ir a parte donde se hace el pan, y completar las cosas que faltan
//     - Centrar el texto
//     - Hacer que el texto pequenio se vea mejor,
//     - Batchear los caracteres en solo una draw call
//     - Hacer un refactor quitando la variable QuadPositionX de globales
//     - Implementar Outlines https://www.redblobgames.com/blog/2024-08-27-sdf-font-outlines/
//     - Escribir con texto chico a la izquierda arriba de las ventanas las teclas y que hace cada una
//     - Agregar kerning

b32 IsRunning = true;

f32 QuadPositionX = 0.0f;

camera Camera;

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

    Renderer* Render = new Renderer(Window);
    Render->Init();

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

        f32 XRel;
        f32 YRel;
        SDL_MouseButtonFlags MouseBtnFlags = SDL_GetRelativeMouseState(&XRel, &YRel);

        if(MouseBtnFlags & SDL_BUTTON_RMASK)
        {
            // TODO(Jsanchez): Control panning speed according to zoom
            // TODO(Jsanchez): Create a mouse file and implement IsPressed(RIGHT_MOUSE_BUTTON);
            // if(IsPressed(RIGHT_MOUSE_BUTTON))
            // {
            // }
            Camera.Position.x -= XRel * 0.2f;
            Camera.Target.x -= XRel * 0.2f;
            Camera.Position.y += YRel * 0.2f;
            Camera.Target.y += YRel * 0.2f;
            Camera.View = glm::lookAt(Camera.Position, Camera.Target, Camera.Up);

            // TODO(Jsanchez): This is leaking memory, create the cursors at init and swap them here, maybe we could
            // swap them at the event handler function, Pressed, Released
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_POINTER));
        }
        else
        {
            SDL_SetCursor(SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT));
        }

        Render->BeginFrame();

        Render->UpdateCamera(&Camera);

        // Update and set rendering variables to quad!
        float Scale = 40.0f;
        glm::vec3 QuadScale = glm::vec3(Scale, Scale, Scale);
        glm::vec3 QuadPosition = {QuadPositionX, 0.0f, 0.0f};
        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::scale(Model, QuadScale);
        Model = glm::translate(Model, QuadPosition);
        i32 ModelLocation = glGetUniformLocation(Render->ExampleShader, "Model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(Model));

        Render->RenderText("abcdefghijklmnñopqrstuvwxyz");

        Render->EndFrame();
    }

    SDL_Quit();
}
