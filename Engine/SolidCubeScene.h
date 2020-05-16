#pragma once
#include "Scene.h"
#include "Cube.h"
#include "PubeScreenTransformer.h"
#include "Colors.h"
#include "ChiliMath.h"

class SolidCubeScene : public Scene
{
public:
    SolidCubeScene() = default;
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw(Graphics &gfx) const override;
private:
    PubeScreenTransformer m_pms;
    Cube m_cube = Cube(1.f);
    static constexpr Color k_colors[12] = {
        Colors::White, Colors::Blue, Colors::Cyan, Colors::Gray,
        Colors::Green, Colors::Magenta, Colors::LightGray, Colors::Red,
        Colors::Yellow, Colors::White, Colors::Blue, Colors::Cyan
    };
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
};