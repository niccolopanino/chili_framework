#pragma once
#include "Vec2.h"
#include "ChiliMath.h"

template<typename T>
class Vec3 : public Vec2<T>
{
public:
    Vec3() { }
    Vec3(T x, T y, T z) : Vec2(x, y), m_z(z) { }
    Vec3(const Vec3 &v) : Vec3(v.m_x, v.m_y, v.m_z) { }

    template<typename TX>
    explicit operator Vec3<TX>() const { return Vec3<TX>((TX)m_x, (TX)m_y, (TX)m_z); }
    Vec3 operator-() const { return Vec3(-m_x, -m_y, -m_z); }
    Vec3 &operator=(const Vec3 &rhs);
    Vec3 operator+(const Vec3 &rhs) const;
    Vec3 &operator+=(const Vec3 &rhs) { return *this = *this + rhs; }
    Vec3 operator-(const Vec3 &rhs) const;
    Vec3 &operator-=(const Vec3 &rhs) { return *this = *this - rhs; }
    Vec3 operator*(T rhs) const { return Vec3(m_x * rhs, m_y * rhs, m_z * rhs); }
    Vec3 &operator*=(T rhs) { return *this = *this * rhs; }
    Vec3 operator*(const Vec3 &rhs) const;
    Vec3 &operator*=(const Vec3 &rhs) { return *this = *this * rhs; }
    Vec3 operator/(T rhs) const { return Vec3(m_x / rhs, m_y / rhs, m_z / rhs); }
    Vec3 &operator/=(T rhs) { return *this = *this / rhs; }
    Vec3 operator/(const Vec3 &rhs) const;
    Vec3 &operator/=(const Vec3 &rhs) { return *this = *this / rhs; }
    bool operator==(const Vec3 &rhs) const;
    bool operator!=(const Vec3 &rhs) const { return !(*this == rhs); }

    T len_sq() const { return sq(m_x) + sq(m_y) + sq(m_z); }
    T len() const { return sqrt(len_sq()); }
    Vec3 get_normalized() const;
    Vec3 &normalize() { return *this = get_normalized(); }
    static T dot(const Vec3 &v1, const Vec3 &v2);
public:
    T m_z = (T)0;
};

typedef Vec3<float>     Vec3f;
typedef Vec3<double>    Vec3d;
typedef Vec3<int>       Vec3i;

template<typename T>
inline Vec3<T> &Vec3<T>::operator=(const Vec3 &rhs)
{
    m_x = rhs.m_x;
    m_y = rhs.m_y;
    m_z = rhs.m_z;
    return *this;
}

template<typename T>
inline Vec3<T> Vec3<T>::operator+(const Vec3 &rhs) const
{
    return Vec3(m_x + rhs.m_x, m_y + rhs.m_y, m_z + rhs.m_z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator-(const Vec3 &rhs) const
{
    return Vec3(m_x - rhs.m_x, m_y - rhs.m_y, m_z - rhs.m_z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator*(const Vec3 &rhs) const
{
    return Vec3(m_x * rhs.m_x, m_y * rhs.m_y, m_z * rhs.m_z);
}

template<typename T>
inline Vec3<T> Vec3<T>::operator/(const Vec3 &rhs) const
{
    return Vec3(m_x / rhs.m_x, m_y / rhs.m_y, m_z / rhs.m_z);
}

template<typename T>
inline bool Vec3<T>::operator==(const Vec3 &rhs) const
{
    return m_x == rhs.m_x && m_y == rhs.m_y && m_z == rhs.m_z;
}

template<typename T>
inline Vec3<T> Vec3<T>::get_normalized() const
{
    const T length = len();
    if (length != (T)0)
        return *this * ((T)1 / length);
    return *this;
}

template<typename T>
inline T Vec3<T>::dot(const Vec3 &v1, const Vec3 &v2)
{
    return v1.m_x * v2.m_x + v1.m_y * v2.m_y + v1.m_z * v2.m_z;
}
