#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "GouraudPointEffect.h"
#include "SolidEffect.h"
#include "IndexedTriangleList.h"
#include "Sphere.h"
#include "ChiliMath.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

class GouraudPointScene : public Scene
{
public:
    typedef ::Pipeline<GouraudPointEffect> Pipeline;
    typedef ::Pipeline<SolidEffect> LightIndicatorPipeline;
    typedef Pipeline::Vertex Vertex;
public:
    GouraudPointScene(Graphics &gfx, IndexedTriangleList<Vertex> itl);
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw() override;
private:
    IndexedTriangleList<Vertex> m_it_list;
    IndexedTriangleList<SolidEffect::Vertex> m_light_indicator;
    Pipeline m_pipeline;
    LightIndicatorPipeline m_light_pipeline;
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
    float m_lpos_x = 0.f;
    float m_lpos_y = 0.f;
    float m_lpos_z = .6f;
};
