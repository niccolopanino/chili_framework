#pragma once
#include "Vec2.h"

class IRect
{
public:
    static IRect from_center(const Vec2i &center, int half_width, int half_height);
public:
    IRect() = default;
    IRect(int left, int right, int top, int bottom);
    IRect(const Vec2i &top_left, const Vec2i &bottom_right);
    IRect(const Vec2i &top_left, int width, int height);
    bool is_overlapping_with(const IRect &other) const;
    bool is_contained_by(const IRect &other) const;
    bool contains(const Vec2i &point) const;
    IRect get_expanded(int offset) const;
    Vec2i get_center() const;
    int get_width() const;
    int get_height() const;
public:
    int m_left;
    int m_right;
    int m_top;
    int m_bottom;
};
