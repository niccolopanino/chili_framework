#pragma once
#include "Color.h"

namespace Colors
{
    static constexpr Color make_rgb(unsigned char r, unsigned char g, unsigned char b)
    {
        return (r << 16) | (g << 8) | b;
    }
    static constexpr Color White = make_rgb(255u, 255u, 255u);
    static constexpr Color Black = make_rgb(0u, 0u, 0u);
    static constexpr Color Gray = make_rgb(0x80u, 0x80u, 0x80u);
    static constexpr Color LightGray = make_rgb(0xD3u, 0xD3u, 0xD3u);
    static constexpr Color Red = make_rgb(255u, 0u, 0u);
    static constexpr Color Green = make_rgb(0u, 255u, 0u);
    static constexpr Color Blue = make_rgb(0u, 0u, 255u);
    static constexpr Color Yellow = make_rgb(255u, 255u, 0u);
    static constexpr Color Cyan = make_rgb(0u, 255u, 255u);
    static constexpr Color Magenta = make_rgb(255u, 0u, 255u);
}
