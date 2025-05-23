#include <fstream>
#include <iostream>

#include <SDL3/SDL.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "renderer.h"
#include "typedefs.h"

// TODO(Jsanchez): Draw the A we are currently drawing but using the data from the loaded font!

// - Dibujar una letra asi como la tengo hoy, pero desde el renderer consumiendo el json
// - Anotar bien para que sirve cada dato de la font data, se que los atlas bounds son los bounds de la letra en la imagen, en pixeles!
// - Dibujar solo 1 character, sin pensar en batching, _usando el json de la font_
// - Al procesar la fuente para el renderer hacer todos los calculos una vez sola, ejemplo: Pasar los atlas bounds de pixels a 0..1, y otros calculos mas que se puedan, si no hay que hacerlos cada frame!
// - Hacer que se dibuje bien utilizando SDF
// - Dibujar un string sin pensar en batching
// - Batchear ese string!!

b32 IsRunning = true;

f32 QuadPositionX = 0.0f;

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
    // TODO(Jsanchez): Aliasing, we might be doing pixel art, we may not want aliasing
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    // TODO(Jsanchez): Turn debug off on release
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

    // TODO: Platform->CreateWindow("Untitled", 1366, 768);
    SDL_Window* Window = SDL_CreateWindow("Untitled", 1366, 768, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

    Renderer* Render = new Renderer(Window);
    Render->Init();

    // Camera Configuration
    // TODO: What type of projection is better for 2D?
    glm::vec3 CameraPosition = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 CameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 View = glm::lookAt(CameraPosition, CameraTarget, CameraUp);
    glm::mat4 Projection = glm::ortho(-240.0f, 240.0f, -135.0f, 135.0f, 0.1f, 100.0f);

    while (IsRunning)
    {
        ProcessEvents();

        Render->BeginFrame();

        // Upload Camera Settings
        i32 ViewLocation = glGetUniformLocation(Render->ExampleShader, "View");
        i32 ProjectionLocation = glGetUniformLocation(Render->ExampleShader, "Projection");
        glUniformMatrix4fv(ViewLocation, 1, GL_FALSE, glm::value_ptr(View));
        glUniformMatrix4fv(ProjectionLocation, 1, GL_FALSE, glm::value_ptr(Projection));

        // Update and set rendering variables to quad!
        glm::vec3 QuadScale = glm::vec3(80.0f, 80.0, 40.0f);
        glm::vec3 QuadPosition = {QuadPositionX, 0.0f, 0.0f};
        glm::mat4 Model = glm::mat4(1.0f);
        Model = glm::scale(Model, QuadScale);
        Model = glm::translate(Model, QuadPosition);
        i32 ModelLocation = glGetUniformLocation(Render->ExampleShader, "Model");
        glUniformMatrix4fv(ModelLocation, 1, GL_FALSE, glm::value_ptr(Model));

        // Draw the quad
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        Render->EndFrame();
    }

    SDL_Quit();
}
