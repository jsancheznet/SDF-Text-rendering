#include "camera.h"

void camera::ApplyZoom(f32 Input)
{
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

    Projection = glm::ortho(-Width, Width, -Height, Height, Near, Far);
}
