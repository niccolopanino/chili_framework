#include "GouraudPointScene.h"
#include "Vec3.h"
#include "Mat3.h"

GouraudPointScene::GouraudPointScene(Graphics &gfx, IndexedTriangleList<Vertex> itl) :
    m_it_list(std::move(itl)), m_pipeline(gfx),
    m_light_indicator(Sphere::get_plain<SolidEffect::Vertex>(.05f)), m_light_pipeline(gfx),
    Scene("Gouraud shader point light scene free mesh")
{
    m_it_list.adjust_to_true_center();
    m_offset_z = m_it_list.get_radius() * 1.6f;
    for (auto &v : m_light_indicator.m_vertices)
        v.m_color = Colors::White;
}

void GouraudPointScene::update(Keyboard & kbd, Mouse & mouse, float dt)
{
    if (kbd.is_key_pressed('Q'))
        m_theta_x = wrap_angle(m_theta_x + k_dtheta * dt);
    if (kbd.is_key_pressed('W'))
        m_theta_y = wrap_angle(m_theta_y + k_dtheta * dt);
    if (kbd.is_key_pressed('E'))
        m_theta_z = wrap_angle(m_theta_z + k_dtheta * dt);
    if (kbd.is_key_pressed('A'))
        m_theta_x = wrap_angle(m_theta_x - k_dtheta * dt);
    if (kbd.is_key_pressed('S'))
        m_theta_y = wrap_angle(m_theta_y - k_dtheta * dt);
    if (kbd.is_key_pressed('D'))
        m_theta_z = wrap_angle(m_theta_z - k_dtheta * dt);
    if (kbd.is_key_pressed('U'))
        m_lpos_x += .2f * dt;
    if (kbd.is_key_pressed('I'))
        m_lpos_y += .2f * dt;
    if (kbd.is_key_pressed('O'))
        m_lpos_z += .2f * dt;
    if (kbd.is_key_pressed('J'))
        m_lpos_x -= .2f * dt;
    if (kbd.is_key_pressed('K'))
        m_lpos_y -= .2f * dt;
    if (kbd.is_key_pressed('L'))
        m_lpos_z -= .2f * dt;
    if (kbd.is_key_pressed('R'))
        m_offset_z += 2.f * dt;
    if (kbd.is_key_pressed('F'))
        m_offset_z -= 2.f * dt;
}

void GouraudPointScene::draw()
{
    m_pipeline.begin_frame();
    // generate rotation matrix from euler angles, translation from offset
    const Mat3f rot = Mat3f::rotate_x(m_theta_x)
        * Mat3f::rotate_y(m_theta_y)
        * Mat3f::rotate_z(m_theta_z);
    const Vec3f trans = Vec3f(0.f, 0.f, m_offset_z);
    // set pipeline transform
    m_pipeline.m_effect.m_vs.bind_rotation(rot);
    m_pipeline.m_effect.m_vs.bind_translation(trans);
    m_pipeline.m_effect.m_vs.set_light_position(Vec3f(m_lpos_x, m_lpos_y, m_lpos_z));
    // render triangles
    m_pipeline.draw(m_it_list);

    m_light_pipeline.begin_frame();
    m_light_pipeline.m_effect.m_vs.bind_translation(Vec3f(m_lpos_x, m_lpos_y, m_lpos_z));
    m_light_pipeline.m_effect.m_vs.bind_rotation(Mat3f::identity());
    m_light_pipeline.draw(m_light_indicator);
}