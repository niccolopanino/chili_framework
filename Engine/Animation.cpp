#include "Animation.h"

Animation::Animation(int x, int y, int width, int height, int count, const Surface &sprite,
    float hold_time, Color chroma)
    :
    m_sprite(sprite),
    m_hold_time(hold_time),
    m_chroma(chroma)
{
    for (int i = 0; i < count; i++)
        m_frames.emplace_back(x + i * width, x + (i + 1) * width, y, y + height);
}

void Animation::draw(const Vec2i &pos, Graphics &gfx) const
{
    gfx.draw_sprite(pos.m_x, pos.m_y, m_frames[m_cur_frame], m_sprite, m_chroma);
}

void Animation::draw(const Vec2i &pos, Graphics &gfx, const IRect &clip) const
{
    gfx.draw_sprite(pos.m_x, pos.m_y, m_frames[m_cur_frame], clip, m_sprite, m_chroma);
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
