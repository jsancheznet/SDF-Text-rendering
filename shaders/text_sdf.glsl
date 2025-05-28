#ifdef VERTEX_SHADER

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

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
    vec4 TextureColor = vec4(texture(Texture, TexCoord));

    TextureColor.a *= step(0.001, TextureColor.r); // if .r <= 0.001, set alpha to 0, else to 1.0

    FragmentColor = TextureColor;
}

#endif
