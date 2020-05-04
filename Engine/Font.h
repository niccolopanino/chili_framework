#pragma once
#include "Graphics.h"
#include "Surface.h"
#include "Vec2.h"

class Font
{
public:
    Font(const std::string &filename, Color chroma = Colors::White);
    void draw_text(const std::string &text, const Vec2i &pos, Color color, Graphics &gfx) const;
private:
    IRect map_glyph_rect(char c) const;
private:
    // holds the font sheet bitmap data
    Surface m_surf;
    // this gives the dimensions of a glyph in the font sheet
    int m_glyph_width;
    int m_glyph_height;
    // font sheet chroma color
    Color m_chroma;
private:
    // number of rows / columns in the font sheet (this is fixed)
    static constexpr int k_num_columns = 32;
    static constexpr int k_num_rows = 3;
    // start and end drawable character codes
    static constexpr char k_first_char = ' ';
    static constexpr char k_last_char = '~';
};
