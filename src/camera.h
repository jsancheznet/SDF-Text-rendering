#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "typedefs.h"

struct camera
{
    glm::vec3 Position;
    glm::vec3 Target;
    glm::vec3 Up;
    glm::mat4 View;
    glm::mat4 Projection;

    // Projection Config
    f32 Width;
    f32 Height;
    f32 Near;
    f32 Far;
    f32 Zoom = 1.0f;

    void ApplyZoom(f32 Input);
};
