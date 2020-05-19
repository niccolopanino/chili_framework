#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "VertexFlatEffect.h"
#include "IndexedTriangleList.h"
#include "Vec3.h"
#include "ChiliMath.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

class CubeFlatIndependentScene : public Scene
{
public:
    typedef Pipeline<VertexFlatEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;
public:
    CubeFlatIndependentScene(Graphics &gfx);
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw() override;
private:
    IndexedTriangleList<Vertex> m_it_list;
    Pipeline m_pipeline;
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
    float m_phi_x = 0.f;
    float m_phi_y = 0.f;
    float m_phi_z = 0.f;
    Vec3f m_light_dir = Vec3f(.2f, -.5f, 1.f);
};
