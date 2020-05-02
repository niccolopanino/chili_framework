#pragma once

class Color
{
public:
    unsigned int dword;
public:
    constexpr Color() : dword() { }
    constexpr Color(const Color &col) : dword(col.dword) { }
    constexpr Color(unsigned int dw) : dword(dw) { }
    constexpr Color(unsigned char x, unsigned char r, unsigned char g, unsigned char b) :
        dword((x << 24u) | (r << 16u) | (g << 8u) | b)
    { }
    constexpr Color(unsigned char r, unsigned char g, unsigned char b) :
        dword((r << 16u) | (g << 8u) | b)
    { }
    constexpr Color(Color col, unsigned char x) : Color((x << 24u) | col.dword) { }
    Color &operator =(Color color);
    constexpr unsigned char GetX() const;
    constexpr unsigned char GetA() const;
    constexpr unsigned char GetR() const;
    constexpr unsigned char GetG() const;
    constexpr unsigned char GetB() const;
    void SetX(unsigned char x);
    void SetA(unsigned char a);
    void SetR(unsigned char r);
    void SetG(unsigned char g);
    void SetB(unsigned char b);
};
