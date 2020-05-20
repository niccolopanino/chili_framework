#include "GouraudScene.h"

GouraudScene::GouraudScene(Graphics &gfx, IndexedTriangleList<Vertex> itl) :
    m_it_list(std::move(itl)), m_pipeline(gfx), Scene("Gouraud shader scene free mesh")
{
    m_it_list.adjust_to_true_center();
    m_offset_z = m_it_list.get_radius() * 1.6f;
}

void GouraudScene::update(Keyboard & kbd, Mouse & mouse, float dt)
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
        m_phi_x = wrap_angle(m_phi_x + k_dtheta * dt);
    if (kbd.is_key_pressed('I'))
        m_phi_y = wrap_angle(m_phi_y + k_dtheta * dt);
    if (kbd.is_key_pressed('O'))
        m_phi_z = wrap_angle(m_phi_z + k_dtheta * dt);
    if (kbd.is_key_pressed('J'))
        m_phi_x = wrap_angle(m_phi_x - k_dtheta * dt);
    if (kbd.is_key_pressed('K'))
        m_phi_y = wrap_angle(m_phi_y - k_dtheta * dt);
    if (kbd.is_key_pressed('L'))
        m_phi_z = wrap_angle(m_phi_z - k_dtheta * dt);
    if (kbd.is_key_pressed('R'))
        m_offset_z += 2.f * dt;
    if (kbd.is_key_pressed('F'))
        m_offset_z -= 2.f * dt;
}

void GouraudScene::draw()
{
    m_pipeline.begin_frame();
    // generate rotation matrix from euler angles, translation from offset
    const Mat3f rot = Mat3f::rotate_x(m_theta_x)
        * Mat3f::rotate_y(m_theta_y)
        * Mat3f::rotate_z(m_theta_z);
    const Mat3f rot_phi = Mat3f::rotate_x(m_phi_x)
        * Mat3f::rotate_y(m_phi_y)
        * Mat3f::rotate_z(m_phi_z);
    const Vec3f trans = Vec3f(0.f, 0.f, m_offset_z);
    // set pipeline transform
    m_pipeline.m_effect.m_vs.bind_rotation(rot);
    m_pipeline.m_effect.m_vs.bind_translation(trans);
    m_pipeline.m_effect.m_vs.set_light_direction(m_light_dir * rot_phi);
    // render triangles
    m_pipeline.draw(m_it_list);
}
