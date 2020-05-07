#pragma once
#include "Colors.h"
#include <string>
#include <vector>
#include "Rect.h"

class Surface
{
public:
    Surface() = default;
    Surface(const std::string &filename);
    Surface(int width, int height);
    Surface(const Surface &) = default;
    Surface(Surface &&) noexcept;
    Surface &operator=(const Surface &) = default;
    Surface &operator=(Surface &&);
    Color get_pixel(int x, int y) const;
    void put_pixel(int x, int y, Color c);
    int get_width() const;
    int get_height() const;
    IRect get_rect() const;
    void fill(Color c);
    const Color *data() const;
private:
    std::vector<Color> m_pixels;
    int m_width = 0;
    int m_height = 0;
};
