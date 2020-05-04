#include "SpriteEffect.h"

SpriteEffect::Chroma::Chroma(Color c) : m_chroma(c)
{ }

void SpriteEffect::Chroma::operator()(int x, int y, Color c, Graphics &gfx) const
{
    if (c != m_chroma)
        gfx.put_pixel(x, y, c);
}

SpriteEffect::Substitution::Substitution(Color c, Color s) : m_chroma(c), m_sub(s)
{ }

void SpriteEffect::Substitution::operator()(int x, int y, Color c, Graphics &gfx) const
{
    if (c != m_chroma)
        gfx.put_pixel(x, y, m_sub);
}

void SpriteEffect::Copy::operator()(int x, int y, Color c, Graphics &gfx) const
{
    gfx.put_pixel(x, y, c);
}

SpriteEffect::Ghost::Ghost(Color c) : SpriteEffect::Ghost(c, 1.f)
{ }

SpriteEffect::Ghost::Ghost(Color c, float alpha) : m_chroma(c), m_alpha(alpha)
{ }

void SpriteEffect::Ghost::operator()(int x, int y, Color c, Graphics &gfx) const
{
    if (c != m_chroma) {
        const Color dst = gfx.get_pixel(x, y);
        const Color blend = Color(
            unsigned char(c.get_r() * m_alpha + dst.get_r() * (1.f - m_alpha)),
            unsigned char(c.get_g() * m_alpha + dst.get_g() * (1.f - m_alpha)),
            unsigned char(c.get_b() * m_alpha + dst.get_b() * (1.f - m_alpha))
        );
    }
}
