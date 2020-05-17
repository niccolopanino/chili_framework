#pragma once
#include "Scene.h"
#include "SkinnedCube.h"
#include "PubeScreenTransformer.h"
#include <string>

class SkinnedCubeScene : public Scene
{
public:
    SkinnedCubeScene(const std::wstring &filename);
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw(Graphics &gfx) const override;
private:
    PubeScreenTransformer m_pms;
    SkinnedCube m_cube = SkinnedCube(1.f);
    Surface m_sb_tex;
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
};
