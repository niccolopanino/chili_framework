#include "MouseTracker.h"

void MouseTracker::engage(const Vec2i &pos)
{
    m_base = pos;
    m_engaged = true;
}

void MouseTracker::release()
{
    m_engaged = false;
}

Vec2i MouseTracker::move(const Vec2i &pos)
{
    const auto delta = pos - m_base;
    m_base = pos;
    return delta;
}

bool MouseTracker::is_engaged() const
{
    return m_engaged;
}
