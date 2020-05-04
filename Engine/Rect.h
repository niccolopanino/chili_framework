#pragma once
#include "Vec2.h"

template<typename T>
class Rect
{
public:
    Rect(T left, T right, T top, T bottom);
    Rect(const Vec2<T> &top_left, const Vec2<T> &bottom_right);
    Rect(const Vec2<T> &top_left, T width, T height);

    bool is_overlapping_with(const Rect &other) const;
    bool is_contained_by(const Rect &other) const;
    bool contains(const Vec2<T> &point) const;
    Rect from_center(const Vec2<T> &center, T half_width, T half_height);
    Rect get_expanded(T offset) const;
    Vec2<T> get_center() const;
    T get_width() const { return m_right - m_left; }
    T get_height() const { return m_bottom - m_top; }
public:
    T m_left;
    T m_right;
    T m_top;
    T m_bottom;
};

typedef Rect<int> IRect;
typedef Rect<float> FRect;

template<typename T>
inline Rect<T>::Rect(T left, T right, T top, T bottom) :
    m_left(left), m_right(right),
    m_top(top), m_bottom(bottom)
{ }

template<typename T>
inline Rect<T>::Rect(const Vec2<T> &top_left, const Vec2<T> &bottom_right) :
    Rect(top_left.m_x, bottom_right.m_x, top_left.m_y, bottom_right.m_y)
{ }

template<typename T>
inline Rect<T>::Rect(const Vec2<T> &top_left, T width, T height) :
    Rect(top_left, top_left + Vec2<T>(width, height))
{ }

template<typename T>
inline bool Rect<T>::is_overlapping_with(const Rect &other) const
{
    return m_right > other.m_left && m_left < other.m_right
        && m_bottom > other.m_top && m_top < other.m_bottom;
}

template<typename T>
inline bool Rect<T>::is_contained_by(const Rect &other) const
{
    return m_left >= other.m_left && m_right <= other.m_right
        && m_top >= other.m_top && m_bottom <= other.m_bottom;
}

template<typename T>
inline bool Rect<T>::contains(const Vec2<T> &point) const
{
    return point.m_x >= m_left && point.m_x <= m_right
        && point.m_y >= m_top && point.m_y <= m_bottom;
}

template<typename T>
inline Rect<T> Rect<T>::from_center(const Vec2<T> &center, T half_width, T half_height)
{
    const Vec2<T> half(half_width, half_height);
    return Rect(center - half, center + half);
}

template<typename T>
inline Rect<T> Rect<T>::get_expanded(T offset) const
{
    return Rect(m_left - offset, m_right + offset, m_top - offset, m_bottom + offset);
}

template<typename T>
inline Vec2<T> Rect<T>::get_center() const
{
    return Vec2<T>((m_left + m_right) / (T)2, (m_top + m_bottom) / (T)2);
}
