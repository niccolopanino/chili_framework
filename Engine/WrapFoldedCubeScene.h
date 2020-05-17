#pragma once
#include "Scene.h"
#include "WrapFoldedCube.h"
#include "PubeScreenTransformer.h"

class WrapFoldedCubeScene : public Scene
{
public:
    WrapFoldedCubeScene() : Scene("Wrap-Folded Textured Cube") { }
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw(Graphics &gfx) const override;
private:
    PubeScreenTransformer m_pms;
    WrapFoldedCube m_cube = WrapFoldedCube(1.f);
    Surface m_sb_tex = Surface::from_file(L"resources/img/sauron-bhole-100x100.png");
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
};
