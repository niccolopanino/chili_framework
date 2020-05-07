#pragma once
#include "Colors.h"
#include "Graphics.h"

namespace SpriteEffect
{
    class Chroma
    {
    public:
        Chroma(Color c) : m_chroma(c) { }
        void operator()(int x, int y, Color c, Graphics &gfx) const
        {
            if (c != m_chroma)
                gfx.put_pixel(x, y, c);
        }
    private:
        Color m_chroma;
    };

    class Substitution
    {
    public:
        Substitution(Color c, Color s) : m_chroma(c), m_sub(s) { }
        void operator()(int x, int y, Color c, Graphics &gfx) const
        {
            if (c != m_chroma)
                gfx.put_pixel(x, y, m_sub);
        }
    private:
        Color m_chroma = Colors::Magenta;
        Color m_sub;
    };

    class Copy
    {
    public:
        void operator()(int x, int y, Color c, Graphics &gfx) const
        {
            gfx.put_pixel(x, y, c);
        }
    };

    class Ghost
    {
    public:
        Ghost(Color c) : Ghost(c, 1.f) { }
        Ghost(Color c, float alpha) : m_chroma(c), m_alpha(alpha) { }
        void operator()(int x, int y, Color c, Graphics &gfx) const
        {
            if (c != m_chroma) {
                const Color dst = gfx.get_pixel(x, y);
                const Color blend = Color(
                    unsigned char(c.get_r() * m_alpha + dst.get_r() * (1.f - m_alpha)),
                    unsigned char(c.get_g() * m_alpha + dst.get_g() * (1.f - m_alpha)),
                    unsigned char(c.get_b() * m_alpha + dst.get_b() * (1.f - m_alpha))
                );
                gfx.put_pixel(x, y, blend);
            }
        }
    private:
        Color m_chroma;
        float m_alpha;
    };
}
