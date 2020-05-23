#include "SpecularPointScene.h"

SpecularPointScene::SpecularPointScene(Graphics &gfx) :
    m_zb(std::make_shared<ZBuffer>(Graphics::k_screen_width, Graphics::k_screen_height)),
    m_pipeline(gfx, m_zb), m_light_pipe(gfx, m_zb), m_wall_pipe(gfx, m_zb),
    Scene("Phong point shader scene free mesh specular highlights")
{
    // adjust suzanne model
    m_suzanne_itl.adjust_to_true_center();
    // set light sphere colors
    for (auto &v : m_light_itl.m_vertices)
        v.m_color = Colors::White;
    // load ceiling, walls, floor
    m_walls.push_back({
        &m_tceiling,
        Plane::get_skinned_normals<VertexLightTexEffect::Vertex>(20, 20, k_width, k_width, k_tscale_ceiling),
        Mat4f::rotate_x(-PI / 2.f) * Mat4f::translate(0.f, k_height / 2.f, 0.f)
    });
    for (int i = 0; i < 4; i++) {
        m_walls.push_back({
            &m_twall,
            Plane::get_skinned_normals<VertexLightTexEffect::Vertex>(20, 20, k_width, k_height, k_tscale_wall),
            Mat4f::translate(0.f, 0.f, k_width / 2.f) * Mat4f::rotate_y(float(i) * PI / 2.f)
        });
    }
    m_walls.push_back({
        &m_tfloor,
        Plane::get_skinned_normals<VertexLightTexEffect::Vertex>(20, 20, k_width, k_width, k_tscale_floor),
        Mat4f::rotate_x(PI / 2.f) * Mat4f::translate(0.f, -k_height / 2.f, 0.f)
    });
}

void SpecularPointScene::update(Keyboard &kbd, Mouse &mouse, float dt)
{
    m_t += dt;

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

    m_theta_y = wrap_angle(m_t * m_rot_speed);
    m_lpos.m_y = m_light_hamp * sin(wrap_angle(m_t * PI / (2.f * m_light_hamp)));
}

void SpecularPointScene::draw()
{
    m_pipeline.begin_frame();
    const Mat4f proj = Mat4f::project_hfov(k_hfov, k_aspect_ratio, .2f, 6.f);
    const Mat4f view = Mat4f::translate(-m_cam_pos) * m_cam_rot_inv;
    // render suzanne
    m_pipeline.m_effect.m_vs.bind_world_view(
        Mat4f::rotate_x(m_theta_x)
        * Mat4f::rotate_y(m_theta_y)
        * Mat4f::rotate_z(m_theta_z)
        * Mat4f::scale(m_scale)
        * Mat4f::translate(m_model_pos)
        * view
    );
    m_pipeline.m_effect.m_vs.bind_projection(proj);
    m_pipeline.m_effect.m_ps.set_light_pos(m_lpos * view);
    m_pipeline.m_effect.m_ps.set_ambient_light(m_lamb);
    m_pipeline.m_effect.m_ps.set_diffuse_light(m_ldiff);
    m_pipeline.draw(m_suzanne_itl);

    // draw light indicator with different pipeline
    // don't call begin_frame on this pipeline because we wanna keep zbuffer contents
    m_light_pipe.m_effect.m_vs.bind_world_view(Mat4f::translate(m_lpos) * view);
    m_light_pipe.m_effect.m_vs.bind_projection(proj);
    m_light_pipe.draw(m_light_itl);

    // draw walls, ceiling, floor
    m_wall_pipe.m_effect.m_vs.bind_projection(proj);
    m_wall_pipe.m_effect.m_vs.set_light_pos(m_lpos * view);
    m_wall_pipe.m_effect.m_vs.set_ambient_light(m_lamb);
    m_wall_pipe.m_effect.m_vs.set_diffuse_light(m_ldiff);
    for (const auto &w : m_walls) {
        m_wall_pipe.m_effect.m_vs.bind_world_view(w.m_model * view);
        m_wall_pipe.m_effect.m_ps.bind_texture(*w.m_tex);
        m_wall_pipe.draw(w.m_model_itl);
    }

    // draw ripple plane
}
