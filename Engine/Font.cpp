#include "Font.h"
#include <cassert>

Font::Font(const std::string &filename, Color chroma) :
    m_surf(filename),
    m_glyph_width(m_surf.get_width() / k_num_columns),
    m_glyph_height(m_surf.get_height() / k_num_rows),
    m_chroma(chroma)
{
    // verify that bitmap had valid dimensions
    assert(m_glyph_width * k_num_columns == m_surf.get_width());
    assert(m_glyph_height * k_num_rows == m_surf.get_height());
}

void Font::draw_text(const std::string &text, const Vec2i &pos, Color color, Graphics &gfx) const
{
    // cur_pos is the pos that we are drawing to on the screen
    auto cur_pos = pos;
    for (auto c : text)
    {
        // on a newline character, reset x position and move down by 1 glyph height
        if (c == '\n') {
            // carriage return
            cur_pos.m_x = pos.m_x;
            // line feed
            cur_pos.m_y += m_glyph_height;
            // we don't want to advance the character position right for a newline
            continue;
        }

        // only draw characters that are on the font sheet
        // start at k_first_char + 1 because might as well skip ' ' as well
        if (c >= k_first_char + 1 && c <= k_last_char)
            gfx.draw_sprite_substitute(cur_pos.m_x, cur_pos.m_y, color,
                map_glyph_rect(c), m_surf, m_chroma);
        // advance screen pos for next character
        cur_pos.m_x += m_glyph_width;
    }
}

IRect Font::map_glyph_rect(char c) const
{
    assert(c >= k_first_char && c <= k_last_char);
    // font sheet glyphs start at ' ', calculate index into sheet
    const int glyph_index = c - ' ';
    // map 1d glyph index to 2d coordinates
    const int y_glyph = glyph_index / k_num_columns;
    const int x_glyph = glyph_index % k_num_columns;
    // convert the sheet grid coords to pixel coords in sheet
    return IRect({ x_glyph * m_glyph_width, y_glyph * m_glyph_height },
        m_glyph_width, m_glyph_height);
}
