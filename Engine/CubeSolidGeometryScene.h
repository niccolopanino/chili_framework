#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "SolidGeometryEffect.h"
#include "IndexedTriangleList.h"
#include "ChiliMath.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

class CubeSolidGeometryScene : public Scene
{
public:
    typedef Pipeline<SolidGeometryEffect> Pipeline;
    typedef Pipeline::Vertex Vertex;
public:
    CubeSolidGeometryScene(Graphics &gfx);
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
};
