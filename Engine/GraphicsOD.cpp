#include "Graphics.h"

void Graphics::begin_frame()
{
    // clear the sysbuffer
    m_sysbuffer.fill(Colors::Black);
}

Color Graphics::get_pixel(int x, int y) const
{
    return m_sysbuffer.get_pixel(x, y);
}

void Graphics::put_pixel(int x, int y, Color c)
{
    m_sysbuffer.put_pixel(x, y, c);
}