#pragma once
#include "Scene.h"
#include "ConcaveHexahedron.h"
#include "PubeScreenTransformer.h"
#include "Colors.h"
#include "ChiliMath.h"

class ConcaHexWireScene : public Scene
{
public:
    ConcaHexWireScene() : Scene("Concave Hexahedron Filled Wireframe") { }
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw(Graphics &gfx) const override;
private:
    PubeScreenTransformer m_pms;
    ConcaveHexahedron m_hex = ConcaveHexahedron(1.f);
    static constexpr Color k_color_solid = Colors::Gray;
    static constexpr Color k_color_wire = Colors::Magenta;
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
};
