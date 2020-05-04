#include "Animation.h"
#include "SpriteEffect.h"

Animation::Animation(int x, int y, int width, int height, int count, const Surface &sprite,
    float hold_time, Color chroma)
    :
    Animation(x, y, width, height, count, sprite, hold_time, 1.f, chroma)
{ }

Animation::Animation(int x, int y, int width, int height, int count, const Surface &sprite,
    float hold_time, float alpha, Color chroma)
    :
    m_sprite(sprite),
    m_hold_time(hold_time),
    m_alpha(alpha),
    m_chroma(chroma)
{
    for (int i = 0; i < count; i++)
        m_frames.emplace_back(x + i * width, x + (i + 1) * width, y, y + height);
}

void Animation::draw(const Vec2i &pos, Graphics &gfx) const
{
    gfx.draw_sprite(pos.m_x, pos.m_y, m_frames[m_cur_frame], m_sprite,
        SpriteEffect::Ghost(m_chroma, m_alpha));
}

void Animation::draw(const Vec2i &pos, Graphics &gfx, const IRect &clip) const
{
    gfx.draw_sprite(pos.m_x, pos.m_y, m_frames[m_cur_frame], clip, m_sprite,
        SpriteEffect::Ghost(m_chroma, m_alpha));
}

void Animation::draw_color(const Vec2i &pos, Graphics &gfx, Color c) const
{
    gfx.draw_sprite(pos.m_x, pos.m_y, m_frames[m_cur_frame], m_sprite,
        SpriteEffect::Substitution(m_chroma, c));
}

void Animation::update(float dt)
{
    m_cur_frame_time += dt;
    while (m_cur_frame_time >= m_hold_time) {
        advance();
        m_cur_frame_time -= m_hold_time;
    }
}

void Animation::advance()
{
    if (++m_cur_frame >= m_frames.size())
        m_cur_frame = 0;
}
