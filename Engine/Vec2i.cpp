#include "Vec2i.h"
#include <cmath>

Vec2i::Vec2i(int x, int y) : m_x(x), m_y(y)
{ }

Vec2i Vec2i::operator+(const Vec2i &rhs) const
{
    return Vec2i(m_x + rhs.m_x, m_y + rhs.m_y);
}

Vec2i &Vec2i::operator+=(const Vec2i &rhs)
{
    return *this = *this + rhs;
}

Vec2i Vec2i::operator*(int rhs) const
{
    return Vec2i(m_x * rhs, m_y * rhs);
}

Vec2i &Vec2i::operator*=(int rhs)
{
    return *this = *this * rhs;
}

Vec2i Vec2i::operator-(const Vec2i &rhs) const
{
    return Vec2i(m_x - rhs.m_x, m_y - rhs.m_y);
}

Vec2i &Vec2i::operator-=(const Vec2i &rhs)
{
    return *this = *this - rhs;
}

Vec2i Vec2i::operator/(int rhs) const
{
    return Vec2i(m_x / rhs, m_y / rhs);
}

Vec2i &Vec2i::operator/=(int rhs)
{
    return *this = *this / rhs;
}

float Vec2i::get_length() const
{
    return std::sqrt(float(get_length_sq()));
}

int Vec2i::get_length_sq() const
{
    return m_x * m_x + m_y * m_y;
}
