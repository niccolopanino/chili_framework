#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include <cmath>

//------------------------------------------------------------------------------
//---- quadratic matrix class template -----------------------------------------
//------------------------------------------------------------------------------

template<typename T, size_t S>
class Mat
{
public:
    Mat &operator=(const Mat &rhs);
    Mat operator*(T rhs) const;
    Mat &operator*=(T rhs) { return *this = *this * rhs; }
    Mat operator*(const Mat &rhs) const;
    Mat &operator*=(const Mat &rhs) { return *this = *this * rhs; }
    Mat operator/(T rhs) const;
    Mat &operator/=(T rhs) { return *this = *this / rhs; }

    static Mat identity();
    static Mat scale(T factor);
    static Mat rotate_2d(T theta);
    static Mat rotate_x(T theta);
    static Mat rotate_y(T theta);
    static Mat rotate_z(T theta);
    static Mat translate(T x, T y, T z);
    template<typename V>
    static Mat translate(const V &tl) { return translate(tl.m_x, tl.m_y, tl.m_z); }
public:
    // [row][col]
    T m_elements[S][S];
};

//------------------------------------------------------------------------------
//---- some matrix typedefs ----------------------------------------------------
//------------------------------------------------------------------------------

typedef Mat<float, 2> Mat2f;
typedef Mat<double, 2> Mat2d;
typedef Mat<int, 2> Mat2i;
typedef Mat<float, 3> Mat3f;
typedef Mat<double, 3> Mat3d;
typedef Mat<int, 3> Mat3i;
typedef Mat<float, 4> Mat4f;
typedef Mat<double, 4> Mat4d;
typedef Mat<int, 4> Mat4i;

//------------------------------------------------------------------------------
//---- vector matrix multiplication operators ----------------------------------
//------------------------------------------------------------------------------

template<typename T>
inline Vec3<T> operator*(const Vec3<T> &v, const Mat<T, 3> &m)
{
    return Vec3<T>(
        v.m_x * m.m_elements[0][0] + v.m_y * m.m_elements[1][0] + v.m_z * m.m_elements[2][0],
        v.m_x * m.m_elements[0][1] + v.m_y * m.m_elements[1][1] + v.m_z * m.m_elements[2][1],
        v.m_x * m.m_elements[0][2] + v.m_y * m.m_elements[1][2] + v.m_z * m.m_elements[2][2]
    );
}

template<typename T>
inline Vec3<T> &operator*=(Vec3<T> &v, const  Mat<T, 3> &m)
{
    return v = v * m;
}

template<typename T>
inline Vec4<T> operator*(const Vec4<T> &v, const Mat<T, 4> &m)
{
    return Vec4<T>(
        v.m_x * m.m_elements[0][0] + v.m_y * m.m_elements[1][0]
        + v.m_z * m.m_elements[2][0] + v.m_w * m.m_elements[3][0],
        v.m_x * m.m_elements[0][1] + v.m_y * m.m_elements[1][1]
        + v.m_z * m.m_elements[2][1] + v.m_w * m.m_elements[3][1],
        v.m_x * m.m_elements[0][2] + v.m_y * m.m_elements[1][2]
        + v.m_z * m.m_elements[2][2] + v.m_w * m.m_elements[3][2],
        v.m_x * m.m_elements[0][3] + v.m_y * m.m_elements[1][3]
        + v.m_z * m.m_elements[2][3] + v.m_w * m.m_elements[3][3]
    );
}

template<typename T>
inline Vec4<T> &operator*=(Vec4<T> &v, const Mat<T, 4> &m)
{
    return v = v * m;
}

//------------------------------------------------------------------------------
//---- definitions for matrix operators ----------------------------------------
//------------------------------------------------------------------------------

template<typename T, size_t S>
inline Mat<T, S> &Mat<T, S>::operator=(const Mat &rhs)
{
    memcpy(m_elements, rhs.m_elements, sizeof(m_elements));
    return *this;
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::operator*(T rhs) const
{
    Mat out;
    for (size_t i = 0; i < S; i++) {
        for (size_t j = 0; j < S; j++)
            out.m_elements[i][j] = m_elements[i][j] * rhs;
    }
    return out;
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::operator*(const Mat &rhs) const
{
    Mat out;
    for (size_t i = 0; i < S; i++) {
        for (size_t j = 0; j < S; j++) {
            T sum = (T)0;
            for (size_t k = 0; k < S; k++)
                sum += m_elements[i][k] * rhs.m_elements[k][j];
            out.m_elements[i][j] = sum;
        }
    }
    return out;
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::operator/(T rhs) const
{
    Mat out;
    for (size_t i = 0; i < S; i++) {
        for (size_t j = 0; j < S; j++)
            out.m_elements[i][j] = m_elements[i][j] / rhs;
    }
    return out;
}

//------------------------------------------------------------------------------
//---- definitions for matrix factory functions --------------------------------
//------------------------------------------------------------------------------

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::identity()
{
    if constexpr (S == 2)
    {
        return {
            (T)1, (T)0,
            (T)0, (T)1
        };
    }
    else if constexpr (S == 3)
    {
        return {
            (T)1, (T)0, (T)0,
            (T)0, (T)1, (T)0,
            (T)0, (T)0, (T)1
        };
    }
    else if constexpr (S == 4)
    {
        return {
            (T)1, (T)0, (T)0, (T)0,
            (T)0, (T)1, (T)0, (T)0,
            (T)0, (T)0, (T)1, (T)0,
            (T)0, (T)0, (T)0, (T)1
        };
    }
    else
    {
        static_assert(false, "bad matrix dimensions");
    }
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::scale(T factor)
{
    if constexpr (S == 2)
    {
        return {
            factor, (T)0,
            (T)0, factor
        };
    }
    else if constexpr (S == 3)
    {
        return {
            factor, (T)0, (T)0,
            (T)0, factor, (T)0,
            (T)0, (T)0, factor
        };
    }
    else if constexpr (S == 4)
    {
        return {
            factor, (T)0, (T)0, (T)0,
            (T)0, factor, (T)0, (T)0,
            (T)0, (T)0, factor, (T)0,
            (T)0, (T)0, (T)0, (T)1
        };
    }
    else
    {
        static_assert(false, "bad matrix dimensions");
    }
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::rotate_2d(T theta)
{
    if constexpr (S == 2)
    {
        const T c = cos(theta);
        const T s = sin(theta);
        return {
            c, s,
            -s, c
        };
    }
    else
    {
        static_assert(false, "bad matrix dimensions");
    }
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::rotate_x(T theta)
{
    const T c = cos(theta);
    const T s = sin(theta);
    if constexpr (S == 3)
    {
        return {
            (T)1, (T)0, (T)0,
            (T)0, c, s,
            (T)0, -s, c
        };
    }
    else if constexpr (S == 4)
    {
        return {
            (T)1, (T)0, (T)0, (T)0,
            (T)0, c, s, (T)0,
            (T)0, -s, c, (T)0,
            (T)0, (T)0, (T)0, (T)1
        };
    }
    else
    {
        static_assert("bad matrix dimensions");
    }
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::rotate_y(T theta)
{
    const T c = cos(theta);
    const T s = sin(theta);
    if constexpr (S == 3)
    {
        return {
            c, (T)0, -s,
            (T)0, (T)1, (T)0,
            s, (T)0, c
        };
    }
    else if constexpr (S == 4)
    {
        return {
            c, (T)0, -s, (T)0,
            (T)0, (T)1, (T)0, (T)0,
            s, (T)0, c, (T)0,
            (T)0, (T)0, (T)0, (T)1
        };
    }
    else
    {
        static_assert("bad matrix dimensions");
    }
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::rotate_z(T theta)
{
    const T c = cos(theta);
    const T s = sin(theta);
    if constexpr (S == 3)
    {
        return {
            c, s, (T)0,
            -s, c, (T)0,
            (T)0, (T)0, (T)1
        };
    }
    else if constexpr (S == 4)
    {
        return {
            c, s, (T)0, (T)0,
            -s, c, (T)0, (T)0,
            (T)0, (T)0, (T)1, (T)0,
            (T)0, (T)0, (T)0, (T)1
        };
    }
    else
    {
        static_assert("bad matrix dimensions");
    }
}

template<typename T, size_t S>
inline Mat<T, S> Mat<T, S>::translate(T x, T y, T z)
{
    if constexpr (S == 4)
    {
        return {
            (T)1, (T)0, (T)0, (T)0,
            (T)0, (T)1, (T)0, (T)0,
            (T)0, (T)0, (T)1, (T)0,
            x, y, z, (T)1
        };
    }
    else
    {
        static_assert("bad matrix dimensions");
    }
}

//------------------------------------------------------------------------------
