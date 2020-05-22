#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "SpecularPointEffect.h"
#include "SolidEffect.h"
#include "VertexLightTexEffect.h"
#include "ZBuffer.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "MouseTracker.h"
#include "IndexedTriangleList.h"
#include "ChiliMath.h"
#include <memory>

class SpecularPointScene : public Scene
{
public:
    typedef ::Pipeline<SpecularPointEffect> Pipeline;
    typedef ::Pipeline<SolidEffect> LightPipeline;
    typedef ::Pipeline<VertexLightTexEffect> WallPipeline;
    typedef Pipeline::Vertex Vertex;
public:
    SpecularPointScene(Graphics &gfx, IndexedTriangleList<Vertex> itl);
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw() override;
private:
    IndexedTriangleList<Vertex> m_it_list;
    IndexedTriangleList<SolidEffect::Vertex> m_light_itl;
    IndexedTriangleList<VertexLightTexEffect::Vertex> m_ceiling_itl;
    std::shared_ptr<ZBuffer> m_zb;
    Pipeline m_pipeline;
    LightPipeline m_light_pipe;
    WallPipeline m_wall_pipe;
    MouseTracker m_mt;
    // fov
    static constexpr float k_aspect_ratio = 1.33333f;
    static constexpr float k_hfov = 95.f;
    static constexpr float k_vfov = k_hfov / k_aspect_ratio;
    // camera stuff
    static constexpr float k_htrack = to_rad(k_hfov) / (float)Graphics::k_screen_width;
    static constexpr float k_vtrack = to_rad(k_vfov) / (float)Graphics::k_screen_height;
    static constexpr float k_cam_speed = 1.f;
    static constexpr float k_cam_roll_speed = PI;
    Vec3f m_cam_pos;
    Mat4f m_cam_rot_inv = Mat4f::identity();
    // model stuff
    Vec3f m_model_pos = Vec3f(0.f, 0.f, 2.f);
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
    // light stuff
    Vec4f m_lpos = Vec4f(0.f, 0.f, .6f, 1.f);
};
