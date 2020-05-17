#include "TexWrapCubeScene.h"
#include "Mat3.h"

TexWrapCubeScene::TexWrapCubeScene(float texdim) :
    TexWrapCubeScene(L"resources/img/sauron-bhole-100x100.png", texdim)
{ }

TexWrapCubeScene::TexWrapCubeScene(const std::wstring &texname, float texdim) :
    m_sb_tex(Surface::from_file(texname)),
    m_cube(1.f, texdim),
    Scene("Textured Cube " + std::string(texname.begin(), texname.end())
        + " Wrapping Dim: " + std::to_string(texdim))
{ }

void TexWrapCubeScene::update(Keyboard &kbd, Mouse &mouse, float dt)
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

void TexWrapCubeScene::draw(Graphics &gfx) const
{
    // generate indexed triangle list
    auto triangles = m_cube.get_textured_triangles();
    // generate rotation matrix from euler angles
    const Mat3f rot = Mat3f::rotate_x(m_theta_x)
        * Mat3f::rotate_y(m_theta_y)
        * Mat3f::rotate_z(m_theta_z);
    // transform from model space to world / view space
    for (auto &v : triangles.m_vertices) {
        v.m_pos *= rot;
        v.m_pos += Vec3f(0.f, 0.f, m_offset_z);
    }
    // backface culling test (must be done in world / view space)
    for (size_t i = 0, end = triangles.m_indices.size() / 3; i < end; i++) {
        const Vec3f &v1 = triangles.m_vertices[triangles.m_indices[i * 3]].m_pos;
        const Vec3f &v2 = triangles.m_vertices[triangles.m_indices[i * 3 + 1]].m_pos;
        const Vec3f &v3 = triangles.m_vertices[triangles.m_indices[i * 3 + 2]].m_pos;
        triangles.m_cull_flags[i] = Vec3f::dot(Vec3f::cross(v2 - v1, v3 - v1), v1) > 0.f;
    }
    // transform to screen space (includes perspective transform)
    for (auto &v : triangles.m_vertices)
        m_pms.transform(v.m_pos);
    // draw the mf triangles!
    for (size_t i = 0, end = triangles.m_indices.size() / 3; i < end; i++) {
        // skip triangles previously determined to be back-facing
        if (!triangles.m_cull_flags[i]) {
            gfx.draw_texture_wrapped_triangle(triangles.m_vertices[triangles.m_indices[i * 3]],
                triangles.m_vertices[triangles.m_indices[i * 3 + 1]],
                triangles.m_vertices[triangles.m_indices[i * 3 + 2]],
                m_sb_tex);
        }
    }
}
