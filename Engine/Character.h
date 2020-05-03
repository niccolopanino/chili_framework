#pragma once
#include "Animation.h"
#include "Vec2f.h"

class Character
{
private:
    enum class Sequence
    {
        WalkingLeft,
        WalkingRight,
        WalkingUp,
        WalkingDown,
        StandingLeft,
        StandingRight,
        StandingUp,
        StandingDown,
        Count
    };
public:
    Character(const Vec2f &pos);
    void draw(Graphics &gfx) const;
    void set_direction(const Vec2f &dir);
    void update(float dt);
private:
    Surface m_sprite;
    Vec2f m_pos;
    Vec2f m_vel = { 0.f, 0.f };
    std::vector<Animation> m_animations;
    Sequence m_cur_seq = Sequence::StandingDown;
    float m_speed = 110.f;
};
