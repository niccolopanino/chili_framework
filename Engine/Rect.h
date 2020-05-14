#pragma once
#include "Vec2.h"
#include <algorithm>

template<typename T>
class Rect
{
public:
    Rect() { }
    Rect(T left, T right, T top, T bottom);
    Rect(const Rect &rec);
    Rect(const Vec2<T> &v1, const Vec2<T> &v2);

    static Rect from_center(const Vec2<T> &center, T half_width, T half_length);

    template<typename TX>
    explicit operator Rect<TX>() const
    {
        return Rect<TX>((TX)m_left, (TX)m_right, (TX)m_top, (TX)m_bottom);
    }

    void translate(T dx, T dy);
    void translate(Vec2<T> v) { translate(v.m_x, v.m_y); }
    void clip_to(const Rect &rec);
    T get_width() const { return m_right - m_left; }
    T get_height() const { return m_bottom - m_top }
    Vec2<T> get_center() const;
    Rect get_expanded(T offset) const;
    bool is_overlapping_with(const Rect &rec) const;
    template<typename TX>
    bool contains(Vec2<TX> v) const;
    template<typename TX>
    bool contains(Rect<TX> rec) const;
public:
    T m_left = (T)0;
    T m_right = (T)0;
    T m_top = (T)0;
    T m_bottom = (T)0;
};

typedef Rect<float>     RectF;
typedef Rect<double>    RectD;
typedef Rect<int>       RectI;

template<typename T>
inline Rect<T>::Rect(T left, T right, T top, T bottom) :
    m_left(left), m_right(right),
    m_top(top), m_bottom(bottom)
{ }

template<typename T>
inline Rect<T>::Rect(const Rect & rec) :
    m_left(rec.m_left), m_right(rec.m_right),
    m_top(rec.m_top), m_bottom(rec.m_bottom)
{ }

template<typename T>
inline Rect<T>::Rect(const Vec2<T> & v1, const Vec2<T> & v2) :
    Rect(min(v1.m_x, v2.m_x), max(v1.m_x, v2.m_x),
         min(v1.m_y, v2.m_y), max(v1.m_y, v2.m_y))
{ }

template<typename T>
inline Rect<T> Rect<T>::from_center(const Vec2<T> & center, T half_width, T half_length)
{
    Vec2<T> half(half_width, half_length);
    return Rect(center - half, center + half);
}

template<typename T>
inline void Rect<T>::translate(T dx, T dy)
{
    m_left += dx;
    m_right += dx;
    m_top += dy;
    m_bottom += dy;
}

template<typename T>
inline void Rect<T>::clip_to(const Rect &rec)
{
    m_left = std::max(m_left, rec.m_left);
    m_right = std::min(m_right, rec.m_right);
    m_top = std::max(m_top, rec.m_top);
    m_bottom = std::min(m_bottom, rec.m_bottom);
}

template<typename T>
inline Vec2<T> Rect<T>::get_center() const
{
    return Vec2<T>((m_left + m_right) / (T)2, (m_top + m_bottom) / (T)2);
}

template<typename T>
inline Rect<T> Rect<T>::get_expanded(T offset) const
{
    return Rect(m_left - offset, m_right + offset, m_top - offset, m_bottom + offset);
}

template<typename T>
inline bool Rect<T>::is_overlapping_with(const Rect &rec) const
{
    return m_left < rec.m_right && m_right > rec.m_left
        && m_top < rec.m_bottom && m_bottom > rec.m_top;
}

template<typename T>
template<typename TX>
inline bool Rect<T>::contains(Vec2<TX> v) const
{
    return m_left <= v.m_x && m_right >= v.m_x
        && m_top <= v.m_y && m_bottom >= v.m_y;
}

template<typename T>
template<typename TX>
inline bool Rect<T>::contains(Rect<TX> rec) const
{
    return m_left <= rec.m_left && m_right >= rec.m_right
        && m_top <= rec.m_top && m_bottom >= rec.m_bottom;
}
