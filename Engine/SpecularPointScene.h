#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "SpecularPointEffect.h"
#include "SolidEffect.h"
#include "ZBuffer.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "IndexedTriangleList.h"
#include "ChiliMath.h"
#include <memory>

class SpecularPointScene : public Scene
{
public:
    typedef ::Pipeline<SpecularPointEffect> Pipeline;
    typedef ::Pipeline<SolidEffect> LightPipeline;
    typedef Pipeline::Vertex Vertex;
public:
    SpecularPointScene(Graphics &gfx, IndexedTriangleList<Vertex> itl);
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw() override;
private:
    IndexedTriangleList<Vertex> m_it_list;
    IndexedTriangleList<SolidEffect::Vertex> m_light_itl;
    std::shared_ptr<ZBuffer> m_zb;
    Pipeline m_pipeline;
    LightPipeline m_light_pipe;
    static constexpr float k_dtheta = PI;
    float m_offset_z = 2.f;
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
    float m_lpos_x = 0.f;
    float m_lpos_y = 0.f;
    float m_lpos_z = .6f;
};
