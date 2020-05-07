#include "Surface.h"
#include <cassert>

Color Surface::get_pixel(int x, int y) const
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    return m_pixels.data()[(size_t)y * m_width + x];
}

void Surface::put_pixel(int x, int y, Color c)
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    m_pixels.data()[(size_t)y * m_width + x] = c;
}

void Surface::fill(Color c)
{
    std::fill(m_pixels.begin(), m_pixels.begin() + (size_t)m_height * m_width, c);
}