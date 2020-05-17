#pragma once
#include "Scene.h"
#include "Cube.h"
#include "PubeScreenTransformer.h"
#include <string>

class TexWrapCubeScene : public Scene
{
public:
    TexWrapCubeScene(float texdim);
    TexWrapCubeScene(const std::wstring &texname, float texdim);
    virtual void update(Keyboard & kbd, Mouse & mouse, float dt) override;
    virtual void draw(Graphics & gfx) const override;
private:
    PubeScreenTransformer m_pms;
    Cube m_cube;
    Surface m_sb_tex;
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
};
