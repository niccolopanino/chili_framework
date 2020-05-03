#include "Character.h"

Character::Character(const Vec2f &pos) : m_sprite("link90x90.bmp"), m_pos(pos)
{
    for (int i = 0; i < (int)Sequence::StandingLeft; i++)
        m_animations.emplace_back(Animation(90, 90 * i, 90, 90, 4, m_sprite, .16f));
    for (int i = (int)Sequence::StandingLeft; i < (int)Sequence::Count; i++)
        m_animations.emplace_back(Animation(0, 90 * (i - (int)Sequence::StandingLeft), 90, 90, 1, m_sprite, .16f));
}

void Character::draw(Graphics &gfx) const
{
    m_animations[(int)m_cur_seq].draw((Vec2i)m_pos, gfx);
}

void Character::set_direction(const Vec2f &dir)
{
    if (dir.m_x > 0.f)
        m_cur_seq = Sequence::WalkingRight;
    else if (dir.m_x < 0.f)
        m_cur_seq = Sequence::WalkingLeft;
    else if (dir.m_y < 0.f)
        m_cur_seq = Sequence::WalkingUp;
    else if (dir.m_y > 0.f)
        m_cur_seq = Sequence::WalkingDown;
    else {
        if (m_vel.m_x > 0.f)
            m_cur_seq = Sequence::StandingRight;
        else if (m_vel.m_x < 0.f)
            m_cur_seq = Sequence::StandingLeft;
        else if (m_vel.m_y < 0.f)
            m_cur_seq = Sequence::StandingUp;
        else if (m_vel.m_y > 0.f)
            m_cur_seq = Sequence::StandingDown;
    }
    m_vel = dir * m_speed;
}

void Character::update(float dt)
{
    m_pos += m_vel * dt;
    m_animations[(int)m_cur_seq].update(dt);
}
