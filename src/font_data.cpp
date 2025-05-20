#include "font_data.h"

#include <fstream>
#include <json.hpp>
#include <sstream>
using json = nlohmann::json;

font_data LoadFontData(std::string FontJson, std::string ImagePath)
{
    // TODO(Jsanchez): Add error handling
    
    font_data Result = {};

    Result.ImagePath = ImagePath;
    
    std::ifstream FileStream(FontJson);
    std::ostringstream Buffer;
    Buffer << FileStream.rdbuf();
    std::string JsonString = Buffer.str().c_str();
    
    json Json = json::parse(JsonString);

    // Atlas
    Result.Type = Json["atlas"]["type"].get<std::string>();
    Result.TextureWidth = Json["atlas"]["width"].get<i32>();
    Result.TextureHeight = Json["atlas"]["height"].get<i32>();
    Result.DistanceRange = Json["atlas"]["distanceRange"].get<i32>();
    Result.DistanceRangeMiddle = Json["atlas"]["distanceRangeMiddle"].get<i32>();
    Result.Size = Json["atlas"]["size"].get<f32>();
    Result.YOrigin = Json["atlas"]["yOrigin"].get<std::string>();

    // Grid
    if(Json.contains("grid"))
    {
        Result.GridCellWidth = Json["atlas"]["grid"]["cellWidth"].get<i32>();
        Result.GridCellHeight = Json["atlas"]["grid"]["cellHeight"].get<i32>();
        Result.GridColumns = Json["atlas"]["grid"]["columns"].get<i32>();
        Result.GridRows = Json["atlas"]["grid"]["rows"].get<i32>();
        Result.GridOriginY = Json["atlas"]["grid"]["originY"].get<f32>();
    }

    // Metrics
    Result.MetricsEmSize = Json["metrics"]["emSize"].get<i32>();
    Result.MetricsLineHeight = Json["metrics"]["lineHeight"].get<f32>();
    Result.MetricsAscender = Json["metrics"]["ascender"].get<f32>();
    Result.MetricsDescender = Json["metrics"]["descender"].get<f32>(); 
    Result.MetricsUnderlineY= Json["metrics"]["underlineY"].get<f32>();
    Result.MetricsThickness = Json["metrics"]["underlineThickness"].get<f32>();

    // Glyphs
    i32 GlyphCount = Json["glyphs"].size();
    for(int  i = 0; i < GlyphCount; ++i)
    {
        glyph Glyph = {};
        
        Glyph.UnicodeId = Json["glyphs"][i]["unicode"].get<i32>();
        Glyph.Advance = Json["glyphs"][i]["advance"].get<f32>();

        if(Json["glyphs"][i].contains("planeBounds"))
        {
            Glyph.PlaneBoundsLeft = Json["glyphs"][i]["planeBounds"]["left"].get<f32>();
            Glyph.PlaneBoundsBottom = Json["glyphs"][i]["planeBounds"]["bottom"].get<f32>();
            Glyph.PlaneBoundsRight = Json["glyphs"][i]["planeBounds"]["right"].get<f32>();
            Glyph.PlaneBoundsTop = Json["glyphs"][i]["planeBounds"]["top"].get<f32>();
        }

        if(Json["glyphs"][i].contains("atlasBounds"))
        {
            Glyph.AtlasBoundsLeft = Json["glyphs"][i]["atlasBounds"]["left"].get<f32>();        
            Glyph.AtlasBoundsBottom = Json["glyphs"][i]["atlasBounds"]["bottom"].get<f32>();        
            Glyph.AtlasBoundsRight = Json["glyphs"][i]["atlasBounds"]["right"].get<f32>();        
            Glyph.AtlasBoundsTop = Json["glyphs"][i]["atlasBounds"]["top"].get<f32>();        
        }

        Result.Glyphs[Glyph.UnicodeId] = Glyph;
    }

    i32 KerningCount = Json["kerning"].size();
    if(KerningCount > 0)
    {
        for(i32 i = 0; i < KerningCount; ++i)
        {
            kerning_info KerningInfo = {};

            KerningInfo.Unicode1 = Json["kerning"][i]["unicode1"].get<i32>();
            KerningInfo.Unicode2 = Json["kerning"][i]["unicode2"].get<i32>();
            KerningInfo.Advance = Json["kerning"][i]["advance"].get<f32>();
        }
    }
    
    return Result;
}