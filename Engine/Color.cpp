#include "Color.h"

Color::Color(const Vec3f &cf) :
    Color(unsigned char(cf.m_x), unsigned char(cf.m_y), unsigned char(cf.m_z))
{ }

Color &Color::operator=(Color color)
{
    m_dword = color.m_dword;
    return *this;
}

bool Color::operator==(const Color &rhs) const
{
    return m_dword ==  rhs.m_dword;
}

bool Color::operator!=(const Color &rhs) const
{
    return !(*this == rhs);
}

void Color::set_x(unsigned char x)
{
    m_dword = (m_dword & 0xFFFFFFu) | (x << 24u);
}

void Color::set_a(unsigned char a)
{
    set_x(a);
}

void Color::set_r(unsigned char r)
{
    m_dword = (m_dword & 0xFF00FFFFu) | (r << 16u);
}

void Color::set_g(unsigned char g)
{
    m_dword = (m_dword & 0xFFFF00FFu) | (g << 8u);
}

void Color::set_b(unsigned char b)
{
    m_dword = (m_dword & 0xFFFFFF00u) | b;
}
