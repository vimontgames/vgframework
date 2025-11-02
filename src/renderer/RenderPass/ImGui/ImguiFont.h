#pragma once

#include "renderer/ImGui_consts.h"

namespace vg::renderer
{
    //--------------------------------------------------------------------------------------
    // Info about stored ImGui font
    //--------------------------------------------------------------------------------------
    struct ImFontInfo
    {
        bool     needed = false;
        bool     failed = false;
        ImFont * ptr = nullptr;
    };

    //--------------------------------------------------------------------------------------
    // A key used to build ImGui font on demand
    //--------------------------------------------------------------------------------------
    class ImFontKey
    {
    public:
        ImFontKey(Font _font = Font::Rowndies, FontStyle _style = FontStyle::Regular, FontSize _size = 16.0f) :
            font(_font),
            style(_style),
            size(_size)
        {

        }

        Font font;
        FontStyle style;
        FontSize size;

        bool operator==(const ImFontKey & other) const noexcept
        {
            return font == other.font && style == other.style && size == other.size;
        }

        struct hash
        {
            inline size_t operator()(const vg::renderer::ImFontKey & key) const noexcept
            {
                size_t h1 = std::hash<vg::renderer::Font>{}(key.font);
                size_t h2 = std::hash<vg::renderer::FontStyle>{}(key.style);
                size_t h3 = std::hash<vg::renderer::FontSize>{}(key.size);

                // Combine hashes — simple and effective:
                size_t seed = h1;
                seed ^= h2 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                seed ^= h3 + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                return seed;
            }
        };
    };
}