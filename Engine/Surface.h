#pragma once
#include "Colors.h"
#include <string>
#include "Rect.h"

class Surface
{
public:
    Surface() = default;
    Surface(const std::string &filename);
    Surface(int width, int height);
    Surface(const Surface &);
    Surface(Surface &&);
    Surface &operator=(const Surface &);
    Surface &operator=(Surface &&);
    ~Surface();
    void put_pixel(int x, int y, Color c);
    Color get_pixel(int x, int y) const;
    int get_width() const;
    int get_height() const;
    IRect get_rect() const;
private:
    Color *m_pixels_ptr = nullptr;
    int m_width = 0;
    int m_height = 0;
};
