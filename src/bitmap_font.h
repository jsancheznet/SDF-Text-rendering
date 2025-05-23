#pragma once

#include <string>
#include <map>
#include <vector>

#include "typedefs.h"

struct glyph
{
    i32 UnicodeId;
    f32 Advance;

    // Plane Bounds
    f32 PlaneBoundsLeft;
    f32 PlaneBoundsBottom;
    f32 PlaneBoundsRight;
    f32 PlaneBoundsTop;

    // Atlas Bounds
    f32 AtlasBoundsLeft;
    f32 AtlasBoundsBottom;
    f32 AtlasBoundsRight;
    f32 AtlasBoundsTop;
};

struct kerning_info
{
    i32 Unicode1;
    i32 Unicode2;
    f32 Advance;
};

struct bitmap_font
{
    u32 TextureHandle;
    std::string ImagePath;

    // Atlas data
    std::string Type;
    std::string YOrigin;
    i32 TextureWidth;
    i32 TextureHeight;
    i32 DistanceRange;
    i32 DistanceRangeMiddle;
    f32 Size;

    // Grid Data
    i32 GridCellWidth;
    i32 GridCellHeight;
    i32 GridColumns;
    i32 GridRows;
    f32 GridOriginY;

    // Metrics
    i32 MetricsEmSize;
    f32 MetricsLineHeight;
    f32 MetricsAscender;
    f32 MetricsDescender;
    f32 MetricsUnderlineY;
    f32 MetricsThickness;

    // Glyphs
    std::map<i32, glyph> Glyphs;

    // Kerning Data
    std::vector<kerning_info> KerningInfo;
};
