#pragma once
#include "Vec2i.h"

class Vec2f
{
public:
    Vec2f() = default;
    Vec2f(float x, float y);
    Vec2f operator+(const Vec2f &rhs) const;
    Vec2f &operator+=(const Vec2f &rhs);
    Vec2f operator*(float rhs) const;
    Vec2f &operator*=(float rhs);
    Vec2f operator-(const Vec2f &rhs) const;
    Vec2f &operator-=(const Vec2f &rhs);
    Vec2f operator/(float rhs) const;
    Vec2f &operator/=(float rhs);
    float get_length() const;
    float get_length_sq() const;
    Vec2f &normalize();
    Vec2f get_normalized() const;
    explicit operator Vec2i() const;
public:
    float m_x;
    float m_y;
};
