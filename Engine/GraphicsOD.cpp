#define GOD_GRAPHICS
#include "Graphics.h"
#include "SpriteEffect.h"
#include <cassert>

void Graphics::begin_frame()
{
    // clear the sysbuffer
    memset(m_sysbuffer, 0u, sizeof(Color) * Graphics::k_screen_width * Graphics::k_screen_height);
}

Color Graphics::get_pixel(int x, int y) const
{
    assert(x >= 0);
    assert(x < Graphics::k_screen_width);
    assert(y >= 0);
    assert(y < Graphics::k_screen_height);
    return m_sysbuffer[Graphics::k_screen_width * y + x];
}

void Graphics::put_pixel(int x, int y, Color c)
{
    assert(x >= 0);
    assert(x < Graphics::k_screen_width);
    assert(y >= 0);
    assert(y < Graphics::k_screen_height);
    m_sysbuffer[Graphics::k_screen_width * y + x] = c;
}

void dummy(Graphics &gfx)
{
    gfx.draw_sprite(0, 0, IRect(0, 0, 0, 0), IRect(0, 0, 0, 0), Surface(), SpriteEffect::Copy());
}
