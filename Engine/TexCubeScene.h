#pragma once
#include "Scene.h"
#include "Cube.h"
#include "PubeScreenTransformer.h"

class TexCubeScene : public Scene
{
public:
    TexCubeScene() = default;
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw(Graphics &gfx) const override;
private:
    PubeScreenTransformer m_pms;
    Cube m_cube = Cube(1.f);
    Surface m_sb_tex = Surface::from_file(L"resources/img/sauron-bhole-100x100.png");
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
};