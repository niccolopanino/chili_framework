#include "ConcaHexScene.h"
#include "Mat3.h"

void ConcaHexScene::update(Keyboard &kbd, Mouse &mouse, float dt)
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

void ConcaHexScene::draw(Graphics &gfx) const
{
    // generate indexed triangle list
    auto triangles = m_hex.get_triangles();
    // generate rotation matrix from euler angles
    const Mat3f rot = Mat3f::rotate_x(m_theta_x)
        * Mat3f::rotate_y(m_theta_y)
        * Mat3f::rotate_z(m_theta_z);
    // transform from model space to world / view space
    for (auto &v : triangles.m_vertices) {
        v *= rot;
        v += Vec3f(0.f, 0.f, m_offset_z);
    }
    // backface culling test (must be done in world / view space)
    for (size_t i = 0, end = triangles.m_indices.size() / 3; i < end; i++) {
        const Vec3f &v1 = triangles.m_vertices[triangles.m_indices[i * 3]];
        const Vec3f &v2 = triangles.m_vertices[triangles.m_indices[i * 3 + 1]];
        const Vec3f &v3 = triangles.m_vertices[triangles.m_indices[i * 3 + 2]];
        triangles.m_cull_flags[i] = Vec3f::dot(Vec3f::cross(v2 - v1, v3 - v1), v1) > 0.f;
    }
    // transform to screen space (includes perspective transform)
    for (auto &v : triangles.m_vertices)
        m_pms.transform(v);
    // draw the mf triangles!
    for (size_t i = 0, end = triangles.m_indices.size() / 3; i < end; i++) {
        // skip triangles previously determined to be back-facing
        if (!triangles.m_cull_flags[i]) {
            gfx.draw_triangle(triangles.m_vertices[triangles.m_indices[i * 3]],
                triangles.m_vertices[triangles.m_indices[i * 3 + 1]],
                triangles.m_vertices[triangles.m_indices[i * 3 + 2]],
                k_colors[i]);
        }
    }
}
