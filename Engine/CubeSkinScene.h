#pragma once
#include "Scene.h"
#include "ChiliMath.h"
#include "Cube.h"
#include "IndexedTriangleList.h"
#include "Graphics.h"
#include "Pipeline.h"
#include "Keyboard.h"
#include "Mouse.h"
#include <string>

class CubeSkinScene : public Scene
{
public:
    typedef Pipeline::Vertex Vertex;
public:
    CubeSkinScene(Graphics &gfx, const std::wstring &filename);
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
