#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "WaveVertexTextureEffect.h"
#include "IndexedTriangleList.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

class VertexWaveScene : public Scene
{
public:
    typedef Pipeline<WaveVertexTextureEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;
public:
    VertexWaveScene(Graphics &gfx);
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
    float m_time = 0.f;
    Vec3f m_light_dir = Vec3f(.2f, -.5f, 1.f);
};
