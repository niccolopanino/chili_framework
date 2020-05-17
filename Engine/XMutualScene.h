#pragma once
#include "Scene.h"
#include "XQuads.h"
#include "PubeScreenTransformer.h"

class XMutualScene : public Scene
{
public:
    XMutualScene() : Scene("Weird X Shape Self Mutual Intersecting Planes Bullshit") { }
    virtual void update(Keyboard & kbd, Mouse & mouse, float dt) override;
    virtual void draw(Graphics & gfx) const override;
private:
    PubeScreenTransformer m_pms;
    XQuads m_xq = XQuads(1.f);
    static constexpr Color k_colors[4] = {
        Colors::Red, Colors::Red,
        Colors::Yellow, Colors::Yellow
    };
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
};
