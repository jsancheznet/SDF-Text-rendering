#include "renderer.h"

#include <cstdio>
#include <iostream>
#include <fstream>
#include <sstream>

#include <SDL3/SDL.h>
#include <glad/glad.h>

#include <json.hpp>
using json = nlohmann::json;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Renderer::Renderer(SDL_Window* Window) : Window(Window) {}

void Renderer::Init()
{
    gladLoadGL();

    { // Enable OpenGL Debug Mode
        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OpenGLDebugMessageCallback, nullptr);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    { // Print extensions list to stdout
        // Get the number of extensions
        GLint numExtensions;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

        // Print all extensions to stdout
#if 0
        for (GLint i = 0; i < numExtensions; i++) {
            const char* extensionName = (const char*)glGetStringi(GL_EXTENSIONS, i);
            std::cout << "Extension #" << i << ": " << extensionName << std::endl;
        }
#endif

    }

    ExampleShader = CompileShader("shaders/text_sdf.glsl");

    DefaultFont = LoadFont("assets/Roboto.json", "assets/Roboto.png");

    // glyph MyGlyph = Roboto.Glyphs['A'];

    // f32 Vertices[] =
    // {
    //     // Positions        // Texture Coordinates
    //     0.5f, 0.5f, 0.0f, MyGlyph.Right, MyGlyph.Top, // top right
    //     0.5f, -0.5f, 0.0f, MyGlyph.Right, MyGlyph.Bottom, // bottom right
    //     -0.5f, -0.5f, 0.0f, MyGlyph.Left, MyGlyph.Bottom, // bottom left
    //     -0.5f, 0.5f, 0.0f, MyGlyph.Left, MyGlyph.Top // top left
    // };

    f32 Vertices[] =
    {
        // Positions        // Texture Coordinates
        0.5f, 0.5f, 0.0f,   0.0f, 0.0f,   // top right
        0.5f, -0.5f, 0.0f,  0.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,   // bottom left
        -0.5f, 0.5f, 0.0f,  0.0f, 0.0f    // top left
    };


    u32 Indices[] =
    {
        0, 1, 3,
        1, 2, 3
    };

    glCreateVertexArrays(1,&VAO);

    glCreateBuffers(1,  &VBO);
    glNamedBufferStorage(VBO, sizeof(Vertices), NULL, GL_DYNAMIC_STORAGE_BIT); // Only allocate memory, do not set it

    glCreateBuffers(1, &EBO);
    glNamedBufferStorage(EBO, sizeof(Indices), Indices, GL_DYNAMIC_STORAGE_BIT);

    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, sizeof(f32) * 5); // f32 *5 is the stride between each vertex
    glVertexArrayElementBuffer(VAO, EBO);

    glEnableVertexArrayAttrib(VAO, 0); // Positions
    glEnableVertexArrayAttrib(VAO, 1); // Texture Coordinates

    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0); // Specify the attrib format for the positions
    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, sizeof(f32) * 3); // Specify the attrib format for the texture coordinates

    glVertexArrayAttribBinding(VAO, 0, 0); // I don't understand the last parameter, WTF? Why always 0?
    glVertexArrayAttribBinding(VAO, 1, 0);

    glBindTextureUnit(0, DefaultFont.Texture);
}

void Renderer::BeginFrame()
{
    glClearColor(0.392f, 0.584f, 0.929f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);

    glUseProgram(ExampleShader);
}

void Renderer::EndFrame()
{
    SDL_GL_SwapWindow(Window);
}

u32 Renderer::CompileShader(const char* Filename)
{
    Assert(Filename);

    size_t Size;
    char *FileString = static_cast<char*>(SDL_LoadFile(Filename, &Size));

    u32 VertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char *VertexSource[2] = {"#version 460 core\n#define VERTEX_SHADER\n", FileString};
    glShaderSource(VertexShader, 2, VertexSource, NULL);
    glCompileShader(VertexShader);
    i32 Compiled;
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(VertexShader, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        VertexShader = 0;
    }

    u32 FragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *FragmentSource[2] = {"#version 460 core\n#define FRAGMENT_SHADER\n", FileString};
    glShaderSource(FragmentShader, 2, FragmentSource, NULL);
    glCompileShader(FragmentShader);
    glGetShaderiv(FragmentShader, GL_COMPILE_STATUS, &Compiled);
    if (Compiled != GL_TRUE)
    {
        i32 LogLength = 0;
        char ErrorMessage[1024];
        glGetShaderInfoLog(FragmentShader, 1024, &LogLength, ErrorMessage);
        fprintf(stderr, "%s-%s\n", Filename, ErrorMessage);
        FragmentShader = 0;
    }

    u32 CompiledShader = glCreateProgram();
    glAttachShader(CompiledShader, VertexShader);
    glAttachShader(CompiledShader, FragmentShader);
    glLinkProgram(CompiledShader);
    i32 IsLinked = 0;
    glGetProgramiv(CompiledShader, GL_LINK_STATUS, (GLint *)&IsLinked);
    if (IsLinked == GL_FALSE)
    {
        i32 MaxLogLength = 1024;
        char InfoLog[1024] = {0};
        glGetProgramInfoLog(CompiledShader, MaxLogLength, &MaxLogLength, &InfoLog[0]);
        printf("%s: SHADER PROGRAM FAILED TO COMPILE/LINK\n", Filename);
        printf("%s\n", InfoLog);
        glDeleteProgram(CompiledShader);
        CompiledShader = 0;
    }

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);
    SDL_free(FileString);

    return CompiledShader;
}

u32 Renderer::CreateTexture(const char* Filepath)
{
    u32 Handle;

    i32 Width, Height, ChannelCount;
    stbi_set_flip_vertically_on_load(true);
    u8 *ImageData = stbi_load(Filepath, &Width, &Height, &ChannelCount, 4);

    glCreateTextures(GL_TEXTURE_2D, 1, &Handle);

    glTextureParameteri(Handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(Handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(Handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(Handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTextureStorage2D(Handle, 1, GL_RGBA8, Width, Height);

    glTextureSubImage2D(Handle, 0, 0, 0, Width, Height, GL_RGBA, GL_UNSIGNED_BYTE, ImageData);

    glGenerateTextureMipmap(Handle);

    stbi_image_free(ImageData);

    return Handle;
}

bitmap_font Renderer::LoadFont(std::string Json, std::string Image)
{
    bitmap_font Result = {};

    Result.ImagePath = Image;
    Result.Texture = CreateTexture(Image.c_str());

    std::ifstream FileStream(Json);
    std::ostringstream Buffer;
    Buffer << FileStream.rdbuf();
    std::string JsonString = Buffer.str().c_str();

    json JsonObject = json::parse(JsonString);

    // Atlas
    Result.Type = JsonObject["atlas"]["type"].get<std::string>();
    Result.TextureWidth = JsonObject["atlas"]["width"].get<i32>();
    Result.TextureHeight = JsonObject["atlas"]["height"].get<i32>();
    Result.DistanceRange = JsonObject["atlas"]["distanceRange"].get<i32>();
    Result.DistanceRangeMiddle = JsonObject["atlas"]["distanceRangeMiddle"].get<i32>();
    Result.Size = JsonObject["atlas"]["size"].get<f32>();
    Result.YOrigin = JsonObject["atlas"]["yOrigin"].get<std::string>();

    // Grid
    if(JsonObject.contains("grid"))
    {
        Result.GridCellWidth = JsonObject["atlas"]["grid"]["cellWidth"].get<i32>();
        Result.GridCellHeight = JsonObject["atlas"]["grid"]["cellHeight"].get<i32>();
        Result.GridColumns = JsonObject["atlas"]["grid"]["columns"].get<i32>();
        Result.GridRows = JsonObject["atlas"]["grid"]["rows"].get<i32>();
        Result.GridOriginY = JsonObject["atlas"]["grid"]["originY"].get<f32>();
    }

    // Metrics
    Result.MetricsEmSize = JsonObject["metrics"]["emSize"].get<i32>();
    Result.MetricsLineHeight = JsonObject["metrics"]["lineHeight"].get<f32>();
    Result.MetricsAscender = JsonObject["metrics"]["ascender"].get<f32>();
    Result.MetricsDescender = JsonObject["metrics"]["descender"].get<f32>();
    Result.MetricsUnderlineY= JsonObject["metrics"]["underlineY"].get<f32>();
    Result.MetricsThickness = JsonObject["metrics"]["underlineThickness"].get<f32>();

    // Glyphs
    i32 GlyphCount = JsonObject["glyphs"].size();
    for(int  i = 0; i < GlyphCount; ++i)
    {
        glyph Glyph = {};

        Glyph.UnicodeId = JsonObject["glyphs"][i]["unicode"].get<i32>();
        Glyph.Advance = JsonObject["glyphs"][i]["advance"].get<f32>();

        if(JsonObject["glyphs"][i].contains("planeBounds"))
        {
            Glyph.PlaneLeft = JsonObject["glyphs"][i]["planeBounds"]["left"].get<f32>();
            Glyph.PlaneBottom = JsonObject["glyphs"][i]["planeBounds"]["bottom"].get<f32>();
            Glyph.PlaneRight = JsonObject["glyphs"][i]["planeBounds"]["right"].get<f32>();
            Glyph.PlaneTop = JsonObject["glyphs"][i]["planeBounds"]["top"].get<f32>();
        }

        if(JsonObject["glyphs"][i].contains("atlasBounds"))
        {
            Glyph.Left = JsonObject["glyphs"][i]["atlasBounds"]["left"].get<f32>() / Result.TextureWidth;
            Glyph.Right = JsonObject["glyphs"][i]["atlasBounds"]["right"].get<f32>() / Result.TextureWidth;
            Glyph.Bottom = JsonObject["glyphs"][i]["atlasBounds"]["bottom"].get<f32>() / Result.TextureHeight;
            Glyph.Top = JsonObject["glyphs"][i]["atlasBounds"]["top"].get<f32>() / Result.TextureHeight;
        }

        Result.Glyphs[Glyph.UnicodeId] = Glyph;
    }

    i32 KerningCount = JsonObject["kerning"].size();
    if(KerningCount > 0)
    {
        for(i32 i = 0; i < KerningCount; ++i)
        {
            kerning_info KerningInfo = {};

            KerningInfo.Unicode1 = JsonObject["kerning"][i]["unicode1"].get<i32>();
            KerningInfo.Unicode2 = JsonObject["kerning"][i]["unicode2"].get<i32>();
            KerningInfo.Advance = JsonObject["kerning"][i]["advance"].get<f32>();
        }
    }

    return Result;
}

void Renderer::RenderText(std::string Text)
{
    glyph Glyph = DefaultFont.Glyphs[Text[0]];

    f32 Vertices[] =
    {
        // Positions        // Texture Coordinates
        0.5f, 0.5f, 0.0f, Glyph.Right, Glyph.Top, // top right
        0.5f, -0.5f, 0.0f, Glyph.Right, Glyph.Bottom, // bottom right
        -0.5f, -0.5f, 0.0f, Glyph.Left, Glyph.Bottom, // bottom left
        -0.5f, 0.5f, 0.0f, Glyph.Left, Glyph.Top // top left
    };

    glNamedBufferSubData(VBO, 0, sizeof(Vertices), Vertices);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Renderer::OpenGLDebugMessageCallback(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, GLchar const* Message, void const* UserParam)
{
    using namespace std;

    switch (Source)
    {
        case GL_DEBUG_SOURCE_API: cout << "API:"; break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM: cout << "WINDOW SYSTEM:"; break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER: cout << "SHADER COMPILER:"; break;
        case GL_DEBUG_SOURCE_THIRD_PARTY: cout << "THIRD PARTY:"; break;
        case GL_DEBUG_SOURCE_APPLICATION: cout << "APPLICATION:"; break;
        case GL_DEBUG_SOURCE_OTHER: cout << "OTHER:"; break;
    }

    switch (Type)
    {
        case GL_DEBUG_TYPE_ERROR: cout << "ERROR:"; break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: cout << "DEPRECATED_BEHAVIOR:"; break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: cout << "UNDEFINED_BEHAVIOR:"; break;
        case GL_DEBUG_TYPE_PORTABILITY: cout << "PORTABILITY:"; break;
        case GL_DEBUG_TYPE_PERFORMANCE: cout << "PERFORMANCE:"; break;
        case GL_DEBUG_TYPE_MARKER: cout << "MARKER:"; break;
        case GL_DEBUG_TYPE_OTHER: cout << "OTHER:"; break;
    }

    switch (Severity)
    {
        case GL_DEBUG_SEVERITY_NOTIFICATION: cout << "NOTIFICATION:"; break;
        case GL_DEBUG_SEVERITY_LOW: cout << "LOW:"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: cout << "MEDIUM:"; break;
        case GL_DEBUG_SEVERITY_HIGH: cout << "HIGH:"; break;
    }

    cout << "->  " << Message << endl;
}
