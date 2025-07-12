#include "camera.h"

void camera::ApplyZoom(f32 Input)
{
    // Mouse wheel up is positive, 1.0f for a single scroll, 2.0f for 2
    // Mouse wheel down is negative

    if(Input > 0.0f)
    {
        // Zoom in
        Width *= 0.90f;
        Height *= 0.90f;
    }
    else
    {
        // Zoom out
        Width *= 1.1f;
        Height *= 1.1f;
    }



    printf("Zoom: %.3f, Width: %.3f, Height: %.3f\n", Zoom, Width, Height);

    Projection = glm::ortho(-Width, Width, -Height, Height, Near, Far);
}
