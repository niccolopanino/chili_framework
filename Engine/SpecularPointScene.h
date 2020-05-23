#pragma once
#include "Scene.h"
#include "Pipeline.h"
#include "SpecularPointEffect.h"
#include "SolidEffect.h"
#include "VertexLightTexEffect.h"
#include "RippleVertexSpecPhongEffect.h"
#include "ZBuffer.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "MouseTracker.h"
#include "IndexedTriangleList.h"
#include "Mat.h"
#include "Sphere.h"
#include "Plane.h"
#include "ChiliMath.h"
#include <memory>
#include <vector>

struct PointDiffuseParams
{
    static constexpr float k_lin_att = .9f;
    static constexpr float k_quad_att = .6f;
    static constexpr float k_const_att = .682f;
};

struct SpecularParams
{
    static constexpr float k_spec_pow = 30.f;
    static constexpr float k_spec_in = .6f;
};

class SpecularPointScene : public Scene
{
    using SpecularPointEffect = SpecularPointEffect<PointDiffuseParams, SpecularParams>;
    using VertexLightTexEffect = VertexLightTexEffect<PointDiffuseParams>;
    using RippleVertexSpecPhongEffect =
        RippleVertexSpecPhongEffect<PointDiffuseParams, SpecularParams>;
public:
    struct Wall
    {
        const Surface *m_tex;
        IndexedTriangleList<VertexLightTexEffect::Vertex> m_model_itl;
        Mat4f m_model;
    };
public:
    typedef ::Pipeline<SpecularPointEffect> Pipeline;
    typedef ::Pipeline<SolidEffect> LightPipeline;
    typedef ::Pipeline<VertexLightTexEffect> WallPipeline;
    typedef ::Pipeline<RippleVertexSpecPhongEffect> RipplePipeline;
    typedef Pipeline::Vertex Vertex;
public:
    SpecularPointScene(Graphics &gfx);
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) override;
    virtual void draw() override;
private:
    float m_time = 0.f;
    // scene params
    static constexpr float k_width = 4.f;
    static constexpr float k_height = 1.75f;
    // pipelines
    std::shared_ptr<ZBuffer> m_zb;
    Pipeline m_pipeline;
    LightPipeline m_light_pipe;
    WallPipeline m_wall_pipe;
    RipplePipeline m_rip_pipe;
    // fov
    static constexpr float k_aspect_ratio = 1.33333f;
    static constexpr float k_hfov = 85.f;
    static constexpr float k_vfov = k_hfov / k_aspect_ratio;
    // camera stuff
    static constexpr float k_htrack = to_rad(k_hfov) / (float)Graphics::k_screen_width;
    static constexpr float k_vtrack = to_rad(k_vfov) / (float)Graphics::k_screen_height;
    static constexpr float k_cam_speed = 1.f;
    static constexpr float k_cam_roll_speed = PI;
    MouseTracker m_mt;
    Vec3f m_cam_pos;
    Mat4f m_cam_rot_inv = Mat4f::identity();
    // suzanne stuff
    IndexedTriangleList<Vertex> m_suzanne_itl =
        IndexedTriangleList<Vertex>::load_normals("resources/models/suzanne.obj");
    Vec3f m_model_pos = Vec3f(1.2f, -.4f, 1.2f);
    float m_theta_x = 0.f;
    float m_theta_y = 0.f;
    float m_theta_z = 0.f;
    float m_rot_speed = PI / 4.f;
    float m_scale = .4f;
    // light stuff
    IndexedTriangleList<SolidEffect::Vertex> m_light_itl =
        Sphere::get_plain<SolidEffect::Vertex>(.05f);
    static constexpr float m_light_hamp = .7f;
    static constexpr float m_light_hperiod = 3.713f;
    Vec4f m_lpos = Vec4f(0.f, 0.f, 0.f, 1.f);
    Vec3f m_ldiff = Vec3f(1.f, 1.f, 1.f);
    Vec3f m_lamb = Vec3f(.35f, .35f, .35f);
    // wall stuff
    static constexpr float k_tscale_ceiling = .5f;
    static constexpr float k_tscale_wall = .65f;
    static constexpr float k_tscale_floor = .65f;
    Surface m_tceiling = Surface::from_file(L"resources/img/ceiling.png");
    Surface m_twall = Surface::from_file(L"resources/img/stonewall.png");
    Surface m_tfloor = Surface::from_file(L"resources/img/floor.png");
    std::vector<Wall> m_walls;
    // ripple stuff
    IndexedTriangleList<RippleVertexSpecPhongEffect::Vertex> m_sauron_itl =
        Plane::get_skinned<RippleVertexSpecPhongEffect::Vertex>(50, 10,
            k_sauron_size, k_sauron_size, .6f);
    static constexpr float k_sauron_size = .6f;
    Mat4f m_sauron_model = Mat4f::rotate_x(PI / 2.f) * Mat4f::translate(.3f, -.8f, 0.f);
    Surface m_tsauron = Surface::from_file(L"resources/img/sauron-bhole-100x100.png");
};
