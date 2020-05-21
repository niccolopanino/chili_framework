#pragma once
#include "Vec3.h"
#include <algorithm>

//------------------------------------------------------------------------------
//---- 4 element vector class template -----------------------------------------
//------------------------------------------------------------------------------

template<typename T>
class Vec4 : public Vec3<T>
{
public:
    Vec4() = default;
    Vec4(T x, T y, T z, T w) : Vec3(x, y, z), m_w(w) { }
    Vec4(const Vec3 &v, T w = (T)1) : Vec3(v), m_w(w) { }
    Vec4(const Vec4 &v) : Vec4(v.m_x, v.m_y, v.m_z, v.m_w) { }
    template<typename TX>
    explicit operator Vec4<TX>() const { return Vec4<TX>((TX)m_x, (TX)m_y, (TX)m_z, (TX)m_w); }
    Vec4 operator-() const { return Vec4(-m_x, -m_y, -m_z, -m_w); }
    Vec4 &operator=(const Vec4 &rhs);
    Vec4 operator+(const Vec4 &rhs) const;
    Vec4 &operator+=(const Vec4 &rhs) { return *this = *this + rhs; }
    Vec4 operator-(const Vec4 &rhs) const;
    Vec4 &operator-=(const Vec4 &rhs) { return *this = *this - rhs; }
    Vec4 operator*(T rhs) const;
    Vec4 &operator*=(T rhs) { return *this = *this * rhs; }
    Vec4 operator*(const Vec4 &rhs) const;
    Vec4 &operator*=(const Vec4 &rhs) { return *this = *this * rhs; }
    Vec4 operator/(T rhs) const;
    Vec4 &operator/=(T rhs) { return *this = *this / rhs; }
    Vec4 operator/(const Vec4 &rhs) const;
    Vec4 &operator/=(const Vec4 &rhs) { return *this = *this / rhs; }
    bool operator==(const Vec4 &rhs) const;
    bool operator!=(const Vec4 &rhs) const { return !(*this == rhs); }

    Vec4 get_saturated() const;
    Vec4 saturate() { *this = get_saturated(); }
    static T dot(const Vec4 &v0, const Vec4 &v1);
public:
    T m_w = (T)1;
};

//------------------------------------------------------------------------------
//---- some vector typedefs ----------------------------------------------------
//------------------------------------------------------------------------------

typedef Vec4<float> Vec4f;
typedef Vec4<double> Vec4d;
typedef Vec4<int> Vec4i;

//------------------------------------------------------------------------------
//---- definitions for vector operators ----------------------------------------
//------------------------------------------------------------------------------

template<typename T>
inline Vec4<T> &Vec4<T>::operator=(const Vec4 &rhs)
{
    m_x = rhs.m_x;
    m_y = rhs.m_y;
    m_z = rhs.m_z;
    m_w = rhs.m_w;
    return *this;
}

template<typename T>
inline Vec4<T> Vec4<T>::operator+(const Vec4 &rhs) const
{
    return Vec4(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z, m_w + rhs.m_w);
}

template<typename T>
inline Vec4<T> Vec4<T>::operator-(const Vec4 &rhs) const
{
    return Vec4(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z, m_w - rhs.m_w);
}

template<typename T>
inline Vec4<T> Vec4<T>::operator*(T rhs) const
{
    return Vec4(m_x * rhs, m_y * rhs, m_z * rhs, m_w * rhs);
}

template<typename T>
inline Vec4<T> Vec4<T>::operator*(const Vec4 &rhs) const
{
    return Vec4(m_x * rhs.m_x, m_y * rhs.m_y, m_z * rhs.m_z, m_w * rhs.m_w);
}

template<typename T>
inline Vec4<T> Vec4<T>::operator/(T rhs) const
{
    return Vec4(m_x / rhs, m_y / rhs, m_z / rhs, m_w / rhs);
}

template<typename T>
inline Vec4<T> Vec4<T>::operator/(const Vec4 &rhs) const
{
    return Vec4(m_x / rhs.m_x, m_y / rhs.m_y, m_z / rhs.m_z, m_w / rhs.m_w);
}

template<typename T>
inline bool Vec4<T>::operator==(const Vec4 &rhs) const
{
    return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z && m_w == rhs.m_w;
}

//------------------------------------------------------------------------------
//---- definitions for vector methods / functions ------------------------------
//------------------------------------------------------------------------------

template<typename T>
inline Vec4<T> Vec4<T>::get_saturated() const
{
    const T x = std::min(1.f, std::max(0.f, m_x));
    const T y = std::min(1.f, std::max(0.f, m_y));
    const T z = std::min(1.f, std::max(0.f, m_z));
    const T w = std::min(1.f, std::max(0.f, m_w));
    return Vec4(x, y, z, w);
}

template<typename T>
inline T Vec4<T>::dot(const Vec4 &v0, const Vec4 &v1)
{
    return v0.m_x * v1.m_x + v0.m_y * v1.m_y + v0.m_z * v1.m_z + v0.m_w * v1.m_w;
}

//------------------------------------------------------------------------------
