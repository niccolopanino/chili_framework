#include "IRect.h"

IRect IRect::from_center(const Vec2i &center, int half_width, int half_height)
{
    const Vec2i half(half_width, half_height);
    return IRect(center - half, center + half);
}

IRect::IRect(int left, int right, int top, int bottom) :
    m_left(left), m_right(right),
    m_top(top), m_bottom(bottom)
{ }

IRect::IRect(const Vec2i &top_left, const Vec2i &bottom_right) :
    IRect(top_left.m_x, bottom_right.m_x, top_left.m_y, bottom_right.m_y)
{ }

IRect::IRect(const Vec2i &top_left, int width, int height) :
    IRect(top_left, top_left + Vec2i(width, height))
{ }

bool IRect::is_overlapping_with(const IRect &other) const
{
    return m_right > other.m_left && m_left < other.m_right
        &&m_bottom > other.m_top && m_top < other.m_bottom;
}

bool IRect::is_contained_by(const IRect &other) const
{
    return m_left >= other.m_left && m_right <= other.m_right
        && m_top >= other.m_top && m_bottom <= other.m_bottom;
}

bool IRect::contains(const Vec2i &point) const
{
    return point.m_x >= m_left && point.m_x < m_right
        &&point.m_y >= m_top && point.m_y < m_bottom;
}

IRect IRect::get_expanded(int offset) const
{
    return IRect(m_left - offset, m_right + offset, m_top - offset, m_bottom + offset);
}

Vec2i IRect::get_center() const
{
    return Vec2i((m_left + m_right) / 2, (m_top + m_bottom) / 2);
}

int IRect::get_width() const
{
    return m_right - m_left;
}

int IRect::get_height() const
{
    return m_bottom - m_top;
}
