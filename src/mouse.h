#pragma once

#include <SDL3/SDL_mouse.h>

#include "typedefs.h"

enum mouse_button
{
    LEFT_BUTTON = SDL_BUTTON_LEFT,
    RIGHT_BUTTON = SDL_BUTTON_RIGHT,
    MIDDLE_BUTTON = SDL_BUTTON_MIDDLE,
};

struct mouse
{
    SDL_MouseButtonFlags ButtonState;     // This frame button state
    SDL_MouseButtonFlags PrevButtonState; // Previous frame button state

    f32 WindowPosX;
    f32 WindowPosY;

    f32 RelativePosX;
    f32 RelativePosY;

    SDL_Cursor *DefaultCursor;
    SDL_Cursor *GrabbingCursor;

    void Init();
    void UpdateState();
    void SetCursor(SDL_Cursor* Cursor);
    b32 IsPressed(mouse_button Button);
};
