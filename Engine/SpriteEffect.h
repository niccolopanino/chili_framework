#pragma once
#include "Colors.h"
#include "Graphics.h"

namespace SpriteEffect
{
    class Chroma
    {
    public:
        Chroma(Color c);
        void operator()(int x, int y, Color c, Graphics &gfx) const;
    private:
        Color m_chroma;
    };

    class Substitution
    {
    public:
        Substitution(Color c, Color s);
        void operator()(int x, int y, Color c, Graphics &gfx) const;
    private:
        Color m_chroma = Colors::Magenta;
        Color m_sub;
    };

    class Copy
    {
    public:
        void operator()(int x, int y, Color c, Graphics &gfx) const;
    };

    class Ghost
    {
    public:
        Ghost(Color c);
        Ghost(Color c, float alpha);
        void operator()(int x, int y, Color c, Graphics &gfx) const;
    private:
        Color m_chroma;
        float m_alpha;
    };
}
