#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <glad/glad.h>

static bool IsRunning = true;

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
                    printf("Escape Pressed!\n");
                }
                if(Event.key.key == SDLK_RIGHT)
                {
                    printf("Right Pressed!\n");
                }
                if(Event.key.key == SDLK_LEFT)
                {
                    printf("Left Pressed!\n");
                }
                if(Event.key.key == SDLK_UP)
                {
                    printf("Up Pressed!\n");
                }
                if(Event.key.key == SDLK_DOWN)
                {
                    printf("Down Pressed!\n");
                }
                break;
            }

            case SDL_EVENT_KEY_UP:
            {
                break;
            }

            case SDL_EVENT_WINDOW_RESIZED:
            {
                // i32 Width = Event.window.data1;
                // i32 Height = Event.window.data2;
                // glViewport(0, 0, Width, Height);
                break;
            }

            default:
            {
                break;
            }
        }
    }
}

int main(int Argc, char **Argv)
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


    SDL_Window *Window = SDL_CreateWindow("Something", 1366, 768, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    SDL_GLContext GLContext = SDL_GL_CreateContext(Window);

    gladLoadGL();

    while(IsRunning)
    {
        ProcessEvents();

        glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        SDL_GL_SwapWindow(Window);
    }

    return 0;
}
