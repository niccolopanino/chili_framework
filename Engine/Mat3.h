#pragma once
#include "Vec3.h"

template<typename T>
class Mat3
{
public:
    Mat3 &operator=(const Mat3 &rhs);
    Mat3 operator*(T rhs) const;
    Mat3 &operator*=(T rhs) { return *this = *this * rhs; }
    Mat3 operator*(const Mat3 &rhs) const;
    Mat3 operator*=(const Mat3 &rhs) { return *this = *this * rhs; }

    static Mat3 identity();
    static Mat3 scale(T factor);
public:
    // [row][col]
    T m_elements[3][3];
};

typedef Mat3<float>     Mat3f;
typedef Mat3<double>    Mat3d;
typedef Mat3<int>       Mat3i;

template<typename T>
inline Mat3<T> &Mat3<T>::operator=(const Mat3 &rhs)
{
    memcpy(m_elements, rhs.m_elements, sizeof(m_elements));
    return *this;
}

template<typename T>
inline Mat3<T> Mat3<T>::operator*(T rhs) const
{
    Mat3 out;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++)
            out[i][j] = this->m_elements[i][j] * rhs;
    }
    return out;
}

template<typename T>
inline Mat3<T> Mat3<T>::operator*(const Mat3 &rhs) const
{
    Mat3 out;
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            T sum = (T)0;
            for (size_t k = 0; k < 3; k++)
                sum += m_elements[i][k] * rhs.m_elements[k][j];
            out.m_elements[i][j] = sum;
        }
    }
    return out;
}

template<typename T>
inline Mat3<T> Mat3<T>::identity()
{
    return {
        (T)1, (T)0, (T)0,
        (T)0, (T)1, (T)0,
        (T)0, (T)0, (T)1
    };
}

template<typename T>
inline Mat3<T> Mat3<T>::scale(T factor)
{
    return {
        factor, (T)0,   (T)0,
        (T)0,   factor, (T)0,
        (T)0,   (T)0,   factor
    };
}

template<typename T>
inline Vec3<T> operator*(const Vec3<T> &v, const Mat3<T> &m)
{
    return Vec3<T>(
        m.m_elements[0][0] * v.m_x + m.m_elements[1][0] * v.m_y + m.m_elements[2][0] * v.m_z,
        m.m_elements[0][1] * v.m_x + m.m_elements[1][1] * v.m_y + m.m_elements[2][1] * v.m_z,
        m.m_elements[0][2] * v.m_x + m.m_elements[1][2] * v.m_y + m.m_elements[2][2] * v.m_z
    );
}

template<typename T>
inline Vec3<T> &operator*=(Vec3<T> &v, const Mat3<T> &m)
{
    return v = v * m;
}
