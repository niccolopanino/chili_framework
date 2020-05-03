#pragma once

class Vec2i
{
public:
    Vec2i() = default;
    Vec2i(int x, int y);
    Vec2i operator+(const Vec2i &rhs) const;
    Vec2i &operator+=(const Vec2i &rhs);
    Vec2i operator*(int rhs) const;
    Vec2i &operator*=(int rhs);
    Vec2i operator-(const Vec2i &rhs) const;
    Vec2i &operator-=(const Vec2i &rhs);
    Vec2i operator/(int rhs) const;
    Vec2i &operator/=(int rhs);
    float get_length() const;
    int get_length_sq() const;
public:
    int m_x;
    int m_y;
};
