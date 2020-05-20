#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "GouraudEffect.h"
#include "Vec3.h"
#include "IndexedTriangleList.h"
#include "ChiliMath.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

class GouraudScene : public Scene
{
public:
    typedef Pipeline<GouraudEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;
public:
    GouraudScene(Graphics &gfx, IndexedTriangleList<Vertex> itl);
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
