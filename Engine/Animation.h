#pragma once
#include "Surface.h"
#include "Graphics.h"
#include <vector>

class Animation
{
public:
    Animation(int x, int y, int width, int height, int count, const Surface &sprite,
        float hold_time, Color chroma = Colors::Magenta);
    void draw(const Vec2i &pos, Graphics &gfx) const;
    void draw(const Vec2i &pos, Graphics &gfx, const IRect &clip) const;
    // this version of draw replaces all opaque pixels with specified color
    void draw_color(const Vec2i &pos, Graphics &gfx, Color c) const;
    void update(float dt);
private:
    void advance();
private:
    Color m_chroma;
    const Surface &m_sprite;
    std::vector<IRect> m_frames;
    int m_cur_frame = 0;
    float m_hold_time;
    float m_cur_frame_time = 0.f;
};
