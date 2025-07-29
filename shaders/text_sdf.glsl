#ifdef VERTEX_SHADER

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;

uniform mat4 Model;
// uniform mat4 View;
// uniform mat4 Projection;

layout(std140, binding = 0) uniform Camera
{
    mat4 View;
    mat4 Projection;
};

out vec2 TexCoord;

void main()
{
    gl_Position = Projection * View * Model * vec4(Position.x, Position.y, Position.z, 1.0);
    TexCoord = vec2(TexCoords.x, TexCoords.y);
}

#endif

#ifdef FRAGMENT_SHADER

in vec2 TexCoord;

out vec4 FragmentColor;
uniform sampler2D Texture;

void main()
{
    float Smoothing = 0.02;
    float Distance = texture(Texture, TexCoord).x;
    float Alpha = smoothstep(0.5 - Smoothing, 0.5 + Smoothing, Distance);
    FragmentColor = vec4(vec3(1.0), Alpha);
}

#endif
