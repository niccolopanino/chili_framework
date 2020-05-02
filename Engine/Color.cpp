#include "Color.h"

Color &Color::operator=(Color color)
{
    dword = color.dword;
    return *this;
}

constexpr unsigned char Color::GetX() const
{
    return dword >> 24u;
}

constexpr unsigned char Color::GetA() const
{
    return GetX();
}

constexpr unsigned char Color::GetR() const
{
    return (dword >> 16u) & 0xFFu;
}

constexpr unsigned char Color::GetG() const
{
    return (dword >> 8u) & 0xFFu;
}

constexpr unsigned char Color::GetB() const
{
    return dword & 0xFFu;
}

void Color::SetX(unsigned char x)
{
    dword = (dword & 0xFFFFFFu) | (x << 24u);
}

void Color::SetA(unsigned char a)
{
    SetX(a);
}

void Color::SetR(unsigned char r)
{
    dword = (dword & 0xFF00FFFFu) | (r << 16u);
}

void Color::SetG(unsigned char g)
{
    dword = (dword & 0xFFFF00FFu) | (g << 8u);
}

void Color::SetB(unsigned char b)
{
    dword = (dword & 0xFFFFFF00u) | b;
}
