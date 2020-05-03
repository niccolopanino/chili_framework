#include "Vec2f.h"
#include <cmath>

Vec2f::Vec2f(float x, float y) : m_x(x), m_y(y)
{ }

Vec2f Vec2f::operator+(const Vec2f &rhs) const
{
    return Vec2f(m_x + rhs.m_x, m_y + rhs.m_y);
}

Vec2f &Vec2f::operator+=(const Vec2f &rhs)
{
    return *this = *this + rhs;
}

Vec2f Vec2f::operator*(float rhs) const
{
    return Vec2f(m_x * rhs, m_y * rhs);
}

Vec2f &Vec2f::operator*=(float rhs)
{
    return *this = *this * rhs;
}

Vec2f Vec2f::operator-(const Vec2f &rhs) const
{
    return Vec2f(m_x - rhs.m_x, m_y - rhs.m_y);
}

Vec2f &Vec2f::operator-=(const Vec2f &rhs)
{
    return *this = *this - rhs;
}

Vec2f Vec2f::operator/(float rhs) const
{
    return Vec2f(m_x / rhs, m_y / rhs);
}

Vec2f &Vec2f::operator/=(float rhs)
{
    return *this = *this / rhs;
}

float Vec2f::get_length() const
{
    return std::sqrt(get_length_sq());
}

float Vec2f::get_length_sq() const
{
    return m_x * m_x + m_y * m_y;
}

Vec2f &Vec2f::normalize()
{
    return *this = get_normalized();
}

Vec2f Vec2f::get_normalized() const
{
    const float len = get_length();
    if (len != 0.f)
        return *this * (1.f / len);
    else
        return *this;
}

Vec2f::operator Vec2i() const
{
    return { int(m_x), int(m_y) };
}
