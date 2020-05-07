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
    void put_pixel(int x, int y, Color c);
    Color get_pixel(int x, int y) const;
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
