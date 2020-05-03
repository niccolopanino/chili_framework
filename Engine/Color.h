#pragma once

class Color
{
public:
    unsigned int m_dword;
public:
    constexpr Color() : m_dword() { }
    constexpr Color(const Color &col) : m_dword(col.m_dword) { }
    constexpr Color(unsigned int dw) : m_dword(dw) { }
    constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) :
        m_dword((x << 24u) | (r << 16u) | (g << 8u) | b)
    { }
    constexpr Color(unsigned char r, unsigned char g, unsigned char b) :
        m_dword((r << 16u) | (g << 8u) | b)
    { }
    constexpr Color(Color col, unsigned char x) : Color((x << 24u) | col.m_dword) { }
    Color &operator =(Color color);
    constexpr unsigned char get_x() const;
    constexpr unsigned char get_a() const;
    constexpr unsigned char get_r() const;
    constexpr unsigned char get_g() const;
    constexpr unsigned char get_b() const;
    void set_x(unsigned char x);
    void set_a(unsigned char a);
    void set_r(unsigned char r);
    void set_g(unsigned char g);
    void set_b(unsigned char b);
};
