#include "mouse.h"

void mouse::Init()
{
    DefaultCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_DEFAULT);
    GrabbingCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_MOVE);
}

void mouse::UpdateState()
{
    PrevButtonState = ButtonState;
    ButtonState = SDL_GetMouseState(&WindowPosX, &WindowPosY);
    SDL_GetRelativeMouseState(&RelativePosX, &RelativePosY);
}

void mouse::SetCursor(SDL_Cursor* Cursor)
{
    if(Cursor)
    {
        SDL_SetCursor(Cursor);
    }
}

b32 mouse::IsPressed(mouse_button Button)
{
    if(ButtonState & SDL_BUTTON_MASK(Button))
    {
        return true;
    }
    else
    {
        return false;
    }
}
