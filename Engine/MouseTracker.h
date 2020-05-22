#pragma once
#include "Vec2.h"

class MouseTracker
{
public:
    void engage(const Vec2i &pos);
    void release();
    Vec2i move(const Vec2i &pos);
    bool is_engaged() const;
private:
    bool m_engaged = false;
    Vec2i m_base;
};
