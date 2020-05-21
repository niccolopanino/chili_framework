#pragma once
#include "ChiliMath.h"

//------------------------------------------------------------------------------
//---- 2 element vector class template -----------------------------------------
//------------------------------------------------------------------------------

template<typename T>
class Vec2
{
public:
    Vec2() = default;
    Vec2(T x, T y) : m_x(x), m_y(y) { }
    Vec2(const Vec2 &v) : Vec2(v.m_x, v.m_y) { }

    template<typename TX>
    explicit operator Vec2<TX>() const { return Vec2<TX>((TX)m_x, (TX)m_y); }
    Vec2 operator-() const { return Vec2(-m_x, -m_y); }
    Vec2 &operator=(const Vec2 &rhs);
    Vec2 operator+(const Vec2 &rhs) const { return Vec2(m_x + rhs.m_x, m_y + rhs.m_y); }
    Vec2 &operator+=(const Vec2 &rhs) { return *this = *this + rhs; }
    Vec2 operator-(const Vec2 &rhs) const { return Vec2(m_x - rhs.m_x, m_y - rhs.m_y); }
    Vec2 &operator-=(const Vec2 &rhs) { return *this = this - rhs; }
    Vec2 operator*(const T rhs) const { return Vec2(m_x * rhs, m_y * rhs); }
    Vec2 &operator*=(const T rhs) { return *this = *this * rhs; }
    Vec2 operator*(const Vec2 &rhs) const { return Vec2(m_x * rhs.m_x, m_y * rhs.m_y); }
    Vec2 &operator*=(const Vec2 &rhs) { return *this = this * rhs; }
    Vec2 operator/(const T rhs) const { return Vec2(m_x / rhs, m_y / rhs); }
    Vec2 &operator/=(const T rhs) { return *this = *this / rhs; }
    Vec2 operator/(const Vec2 &rhs) const { return Vec2(m_x / rhs.m_x, m_y / rhs.m_y); }
    Vec2 &operator/=(const Vec2 &rhs) { return *this = this / rhs; }
    bool operator==(const Vec2 &rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y; }
    bool operator!=(const Vec2 &rhs) const { return !(*this == rhs); }

    T len_sq() const { return sq(m_x) + sq(m_y); }
    T len() const { return sqrt(len_sq()); }
    Vec2 get_normalized() const;
    Vec2 &normalize() { return *this = get_normalized(); }
    static T dot(const Vec2 &v1, const Vec2 &v2) { return v1.m_x * v2.m_x + v1.m_y * v2.m_y; }
public:
    T m_x = (T)0;
    T m_y = (T)0;
};

//------------------------------------------------------------------------------
//---- some vector typedefs ----------------------------------------------------
//------------------------------------------------------------------------------

typedef Vec2<float>     Vec2f;
typedef Vec2<double>    Vec2d;
typedef Vec2<int>       Vec2i;

//------------------------------------------------------------------------------
//---- definitions for vector operators ----------------------------------------
//------------------------------------------------------------------------------

template<typename T>
inline Vec2<T> &Vec2<T>::operator=(const Vec2 &rhs)
{
    m_x = rhs.m_x;
    m_y = rhs.m_y;
    return *this;
}

//------------------------------------------------------------------------------
//---- definitions for vector methods / functions ------------------------------
//------------------------------------------------------------------------------

template<typename T>
inline Vec2<T> Vec2<T>::get_normalized() const
{
    const T length = len();
    if (length != (T)0)
        return *this * ((T)1 / length);
    return *this;
}

//------------------------------------------------------------------------------
