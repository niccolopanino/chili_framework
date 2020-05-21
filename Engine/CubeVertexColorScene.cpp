#include "CubeVertexColorScene.h"
#include "Cube.h"
#include "Mat.h"
#include "Vec3.h"
#include "Colors.h"

CubeVertexColorScene::CubeVertexColorScene(Graphics &gfx) :
    m_it_list(Cube::get_plain<Vertex>()), m_pipeline(gfx),
    Scene("Colored cube vertex gradient scene")
{
    m_it_list.m_vertices[0].m_color = Vec3f(Colors::Red);
    m_it_list.m_vertices[1].m_color = Vec3f(Colors::Green);
    m_it_list.m_vertices[2].m_color = Vec3f(Colors::Blue);
    m_it_list.m_vertices[3].m_color = Vec3f(Colors::Yellow);
    m_it_list.m_vertices[4].m_color = Vec3f(Colors::Cyan);
    m_it_list.m_vertices[5].m_color = Vec3f(Colors::Magenta);
    m_it_list.m_vertices[6].m_color = Vec3f(Colors::White);
    m_it_list.m_vertices[7].m_color = Vec3f(Colors::Black);
}

void CubeVertexColorScene::update(Keyboard & kbd, Mouse & mouse, float dt)
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

void CubeVertexColorScene::draw()
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
    // render triangles
    m_pipeline.draw(m_it_list);
}
