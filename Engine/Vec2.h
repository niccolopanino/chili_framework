#pragma once
#include <cmath>

template<typename T>
class Vec2
{
public:
    Vec2() = default;
    Vec2(T x, T y) : m_x(x), m_y(y) { }
    template<typename TX>
    Vec2(const Vec2<TX> &src) : m_x((T)src.m_x), m_y((T)src.m_y) { }

    Vec2 operator+(const Vec2 &rhs) const { return Vec2(m_x + rhs.m_x, m_y + rhs.m_y); }
    Vec2 operator+=(const Vec2 &rhs) { return *this = *this + rhs; }
    Vec2 operator*(T rhs) const { return Vec2(m_x * rhs, m_y * rhs); }
    Vec2 operator*=(T rhs) { return *this = *this * rhs; }
    Vec2 operator-(const Vec2 &rhs) const { return Vec2(m_x - rhs.m_x, m_y - rhs.m_y); }
    Vec2 operator-=(const Vec2 &rhs) { return *this = *this - rhs; }
    Vec2 operator/(T rhs) const { return Vec2(m_x / rhs, m_y / rhs); }
    Vec2 operator/=(T rhs) { return *this = *this / rhs; }

    T get_length() const { return (T)std::sqrt(get_length_sq()); }
    T get_length_sq() const { return m_x * m_x + m_y * m_y; }
    Vec2 &normalize() { return *this = get_normalized(); }
    Vec2 get_normalized() const;
public:
    T m_x;
    T m_y;
};

typedef Vec2<float> Vec2f;
typedef Vec2<int> Vec2i;

template<typename T>
inline Vec2<T> Vec2<T>::get_normalized() const
{
    const T len = get_length();
    if (len != (T)0)
        return *this * ((T)1 / len);
    return *this;
}
