#pragma once
#include "Vec2.h"

template<typename T>
class Mat2
{
public:
    Mat2 &operator=(const Mat2 &rhs);
    Mat2 operator*(T rhs) const;
    Mat2 &operator*=(T rhs) { return *this = *this * rhs; }
    Mat2 operator*(const Mat2 &rhs) const;
    Mat2 &operator*=(const Mat2 &rhs) { return *this = *this * rhs; }

    static Mat2 identity() { return { (T)1, (T)0, (T)0, (T)1 }; }
    static Mat2 rotation(T theta);
    static Mat2 scaling(T factor) { return { factor, (T)0, (T)0, factor }; }
public:
    // [row][col]
    T m_elements[2][2];
};

typedef Mat2<float>     Mat2f;
typedef Mat2<double>    Mat2d;
typedef Mat2<int>       Mat2i;

template<typename T>
inline Mat2<T> &Mat2<T>::operator=(const Mat2 &rhs)
{
    memcpy(m_elements, rhs.m_elements, sizeof(m_elements));
    return *this;
}

template<typename T>
inline Mat2<T> Mat2<T>::operator*(T rhs) const
{
    Mat2 out;
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++)
            out[i][j] = this->m_elements[i][j] * rhs;
    }
    return out;
}

template<typename T>
inline Mat2<T> Mat2<T>::operator*(const Mat2 &rhs) const
{
    Mat2 out;
    for (size_t i = 0; i < 2; i++) {
        for (size_t j = 0; j < 2; j++) {
            T sum = (T)0;
            for (size_t k = 0; k < 2; k++)
                sum += m_elements[i][k] * rhs.m_elements[k][j];
            out.m_elements[i][j] = sum;
        }
    }
    return out;
}

template<typename T>
inline Mat2<T> Mat2<T>::rotation(T theta)
{
    const T cos_theta = cos(theta);
    const T sin_theta = sin(theta);
    return { cos_theta, sin_theta, -sin_theta, cos_theta };
}

template<typename T>
inline Vec2<T> operator*(const Vec2<T> &lhs, const Mat2<T> &rhs)
{
    return Vec2<T>(rhs.m_elements[0][0] * lhs.m_x + rhs.m_elements[0][1] * lhs.m_y,
        rhs.m_elements[1][0] * lhs.m_x + rhs.m_elements[1][1] * lhs.m_y);
}

template<typename T>
Vec2<T> &operator*=(Vec2<T> &lhs, const Mat2<T> &rhs)
{
    return lhs = lhs * rhs;
}
