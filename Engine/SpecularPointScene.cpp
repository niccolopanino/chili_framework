#include "SpecularPointScene.h"
#include "Mat.h"
#include "Sphere.h"
#include "Plane.h"

SpecularPointScene::SpecularPointScene(Graphics &gfx, IndexedTriangleList<Vertex> itl) :
    m_zb(std::make_shared<ZBuffer>(Graphics::k_screen_width, Graphics::k_screen_height)),
    m_it_list(std::move(itl)), m_light_itl(Sphere::get_plain<SolidEffect::Vertex>(.05f)),
    m_ceiling_itl(Plane::get_skinned_normals<VertexLightTexEffect::Vertex>(20)),
    m_pipeline(gfx, m_zb), m_light_pipe(gfx, m_zb), m_wall_pipe(gfx, m_zb),
    Scene("Phong point shader scene free mesh specular highlights")
{
    m_it_list.adjust_to_true_center();
    m_model_pos.m_z = m_it_list.get_radius() * 1.6f;
    for (auto &v : m_light_itl.m_vertices)
        v.m_color = Colors::White;
    m_wall_pipe.m_effect.m_ps.bind_texture(L"resources/img/ceiling.jpg");
}

void SpecularPointScene::update(Keyboard &kbd, Mouse &mouse, float dt)
{
    if (kbd.is_key_pressed('W'))
        m_cam_pos += Vec4f(0.f, 0.f, 1.f, 0.f) * m_cam_rot_inv.get_transposed() * k_cam_speed * dt;
    if (kbd.is_key_pressed('A'))
        m_cam_pos += Vec4f(-1.f, 0.f, 0.f, 0.f) * m_cam_rot_inv.get_transposed() * k_cam_speed * dt;
    if (kbd.is_key_pressed('S'))
        m_cam_pos += Vec4f(0.f, 0.f, -1.f, 0.f) * m_cam_rot_inv.get_transposed() * k_cam_speed * dt;
    if (kbd.is_key_pressed('D'))
        m_cam_pos += Vec4f(1.f, 0.f, 0.f, 0.f) * m_cam_rot_inv.get_transposed() * k_cam_speed * dt;
    if (kbd.is_key_pressed('C'))
        m_cam_pos += Vec4f(0.f, 1.f, 0.f, 0.f) * m_cam_rot_inv.get_transposed() * k_cam_speed * dt;
    if (kbd.is_key_pressed('Z'))
        m_cam_pos += Vec4f(0.f, -1.f, 0.f, 0.f) * m_cam_rot_inv.get_transposed() * k_cam_speed * dt;
    if (kbd.is_key_pressed('Q'))
        m_cam_rot_inv *= Mat4f::rotate_z(k_cam_roll_speed * dt);
    if (kbd.is_key_pressed('E'))
        m_cam_rot_inv *= Mat4f::rotate_z(-k_cam_roll_speed * dt);

    while (!mouse.is_empty())
    {
        const auto e = mouse.read();
        switch (e.get_type())
        {
            case Mouse::Event::Type::LPress:
                m_mt.engage(e.get_pos());
                break;
            case Mouse::Event::Type::LRelease:
                m_mt.release();
                break;
            case Mouse::Event::Type::Move:
                if (m_mt.is_engaged()) {
                    const auto delta = m_mt.move(e.get_pos());
                    m_cam_rot_inv *= Mat4f::rotate_y((float)-delta.m_x * k_htrack)
                        * Mat4f::rotate_x((float)-delta.m_y * k_vtrack);
                }
                break;
        }
    }
}

void SpecularPointScene::draw()
{
    m_pipeline.begin_frame();
    // set pipeline transform
    const Mat4f proj = Mat4f::project_hfov(k_hfov, k_aspect_ratio, .5f, 4.f);
    const Mat4f view = Mat4f::translate(-m_cam_pos) * m_cam_rot_inv;
    m_pipeline.m_effect.m_vs.bind_world_view(
        Mat4f::rotate_x(m_theta_x)
        * Mat4f::rotate_y(m_theta_y)
        * Mat4f::rotate_z(m_theta_z)
        * Mat4f::translate(m_model_pos)
        * view
    );
    m_pipeline.m_effect.m_vs.bind_projection(proj);
    m_pipeline.m_effect.m_ps.set_light_pos(m_lpos * view);
    // render triangles
    m_pipeline.draw(m_it_list);

    // draw light indicator with different pipeline
    // don't call begin_frame on this pipeline because we wanna keep zbuffer contents
    m_light_pipe.m_effect.m_vs.bind_world_view(Mat4f::translate(m_lpos) * view);
    m_light_pipe.m_effect.m_vs.bind_projection(proj);
    m_light_pipe.draw(m_light_itl);

    m_wall_pipe.m_effect.m_vs.bind_world_view(Mat4f::translate(0.f, 0.f, .4f + m_lpos.m_z) * view);
    m_wall_pipe.m_effect.m_vs.bind_projection(proj);
    m_wall_pipe.m_effect.m_vs.set_light_pos(m_lpos * view);
    m_wall_pipe.draw(m_ceiling_itl);
}
