#include "DoubleCubeScene.h"
#include "Cube.h"
#include "Mat3.h"
#include "Vec3.h"
#include "Colors.h"

DoubleCubeScene::DoubleCubeScene(Graphics &gfx) :
    m_it_list(Cube::get_plain_independent_faces<Vertex>()), m_pipeline(gfx),
    Scene("Double cube interpenetration test scene")
{
    const Color colors[] = {
        Colors::Red, Colors::Green, Colors::Blue,
        Colors::Magenta, Colors::Yellow, Colors::Cyan
    };

    for (int i = 0; i < m_it_list.m_vertices.size(); i++)
        m_it_list.m_vertices[i].m_color = colors[i / 4];
}

void DoubleCubeScene::update(Keyboard &kbd, Mouse &mouse, float dt)
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
    if (kbd.is_key_pressed('R'))
        m_offset_z += 2.f * dt;
    if (kbd.is_key_pressed('F'))
        m_offset_z -= 2.f * dt;
}

void DoubleCubeScene::draw()
{
    m_pipeline.begin_frame();
    // draw fixed cube
    {
        // generate rotation matrix from euler angles, translation from offset
        const Mat3f rot = Mat3f::rotate_x(-m_theta_x)
            * Mat3f::rotate_y(-m_theta_y)
            * Mat3f::rotate_z(-m_theta_z);
        // set pipeline transform
        m_pipeline.m_effect.m_vs.bind_rotation(rot);
        m_pipeline.m_effect.m_vs.bind_translation(Vec3f(0.f, 0.f, 2.f));
        // render triangles
        m_pipeline.draw(m_it_list);
    }
    // draw mobile cube
    {
        // generate rotation matrix from euler angles, translation from offset
        const Mat3f rot = Mat3f::rotate_x(m_theta_x)
            * Mat3f::rotate_y(m_theta_y)
            * Mat3f::rotate_z(m_theta_z);
        // set pipeline transform
        m_pipeline.m_effect.m_vs.bind_rotation(rot);
        m_pipeline.m_effect.m_vs.bind_translation(Vec3f(0.f, 0.f, m_offset_z));
        // render triangles
        m_pipeline.draw(m_it_list);
    }
}
