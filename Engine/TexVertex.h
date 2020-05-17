#pragma once
#include "Vec2.h"
#include "Vec3.h"

class TexVertex
{
public:
    TexVertex(const Vec3f &pos, const Vec2f &tc) : m_pos(pos), m_tc(tc) { }
    TexVertex operator+(const TexVertex &rhs) const;
    TexVertex &operator+=(const TexVertex &rhs) { return *this = *this + rhs; }
    TexVertex operator-(const TexVertex &rhs) const;
    TexVertex &operator-=(const TexVertex &rhs) { return *this = *this - rhs; }
    TexVertex operator*(float rhs) const;
    TexVertex &operator*=(float rhs) { return *this = *this * rhs; }
    TexVertex operator/(float rhs) const;
    TexVertex &operator/=(float rhs) { return *this = *this / rhs; }
    TexVertex interpolate(const TexVertex &dst, float alpha) const;
public:
    Vec3f m_pos;
    Vec2f m_tc;
};
