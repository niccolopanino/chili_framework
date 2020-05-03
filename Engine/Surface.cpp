#include "Surface.h"
#include <cassert>

Surface::Surface(int width, int height) :
    m_width(width), m_height(height),
    m_pixels_ptr(new Color[width * height])
{ }

Surface::Surface(const Surface &rhs) : Surface(rhs.m_width, rhs.m_height)
{
    const int num_pixels = m_width * m_height;
    for (int i = 0; i < num_pixels; i++) {
        m_pixels_ptr[i] = rhs.m_pixels_ptr[i];
    }
}

Surface &Surface::operator=(const Surface &rhs)
{
    m_width = rhs.m_width;
    m_height = rhs.m_height;
    delete[] m_pixels_ptr;
    m_pixels_ptr = new Color[m_width * m_height];
    const int num_pixels = m_width * m_height;
    for (int i = 0; i < num_pixels; i++) {
        m_pixels_ptr[i] = rhs.m_pixels_ptr[i];
    }
    return *this;
}

Surface::~Surface()
{
    delete[] m_pixels_ptr;
    m_pixels_ptr = nullptr;
}

void Surface::put_pixel(int x, int y, Color c)
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    m_pixels_ptr[y * m_width + x] = c;
}

Color Surface::get_pixel(int x, int y) const
{
    assert(x >= 0);
    assert(x < m_width);
    assert(y >= 0);
    assert(y < m_height);
    return m_pixels_ptr[y * m_width + x];
}

int Surface::get_width() const
{
    return m_width;
}

int Surface::get_height() const
{
    return m_height;
}
