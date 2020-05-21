//#include "PhongPointScene.h"
//#include "Vec3.h"
//#include "Mat.h"
//#include "Colors.h"
//#include "Sphere.h"
//#include <utility>
//
//PhongPointScene::PhongPointScene(Graphics &gfx, IndexedTriangleList<Vertex> itl) :
//    m_zb(std::make_shared<ZBuffer>(Graphics::k_screen_width, Graphics::k_screen_height)),
//    m_it_list(std::move(itl)), m_light_itl(Sphere::get_plain<SolidEffect::Vertex>(.05f)),
//    m_pipeline(gfx, m_zb), m_light_pipe(gfx, m_zb),
//    Scene("Phong point shader scene free mesh")
//{
//    m_it_list.adjust_to_true_center();
//    m_offset_z = m_it_list.get_radius() * 1.6f;
//    for (auto &v : m_light_itl.m_vertices)
//        v.m_color = Colors::White;
//}
//
//void PhongPointScene::update(Keyboard &kbd, Mouse &mouse, float dt)
//{
//    if (kbd.is_key_pressed('Q'))
//        m_theta_x = wrap_angle(m_theta_x + k_dtheta * dt);
//    if (kbd.is_key_pressed('W'))
//        m_theta_y = wrap_angle(m_theta_y + k_dtheta * dt);
//    if (kbd.is_key_pressed('E'))
//        m_theta_z = wrap_angle(m_theta_z + k_dtheta * dt);
//    if (kbd.is_key_pressed('A'))
//        m_theta_x = wrap_angle(m_theta_x - k_dtheta * dt);
//    if (kbd.is_key_pressed('S'))
//        m_theta_y = wrap_angle(m_theta_y - k_dtheta * dt);
//    if (kbd.is_key_pressed('D'))
//        m_theta_z = wrap_angle(m_theta_z - k_dtheta * dt);
//    if (kbd.is_key_pressed('U'))
//        m_lpos_x += .2f * dt;
//    if (kbd.is_key_pressed('I'))
//        m_lpos_y += .2f * dt;
//    if (kbd.is_key_pressed('O'))
//        m_lpos_z += .2f * dt;
//    if (kbd.is_key_pressed('J'))
//        m_lpos_x -= .2f * dt;
//    if (kbd.is_key_pressed('K'))
//        m_lpos_y -= .2f * dt;
//    if (kbd.is_key_pressed('L'))
//        m_lpos_z -= .2f * dt;
//    if (kbd.is_key_pressed('R'))
//        m_offset_z += 2.f * dt;
//    if (kbd.is_key_pressed('F'))
//        m_offset_z -= 2.f * dt;
//}
//
//void PhongPointScene::draw()
//{
//    m_pipeline.begin_frame();
//    // generate rotation matrix from euler angles, translation from offset
//    const Mat3f rot = Mat3f::rotate_x(m_theta_x)
//        * Mat3f::rotate_y(m_theta_y)
//        * Mat3f::rotate_z(m_theta_z);
//    const Vec3f trans = Vec3f(0.f, 0.f, m_offset_z);
//    // set pipeline transform
//    m_pipeline.m_effect.m_vs.bind_rotation(rot);
//    m_pipeline.m_effect.m_vs.bind_translation(trans);
//    m_pipeline.m_effect.m_ps.set_light_position(Vec3f(m_lpos_x, m_lpos_y, m_lpos_z));
//    // render triangles
//    m_pipeline.draw(m_it_list);
//
//    // draw light indicator with different pipeline
//    // don't call begin_frame on this pipeline because we wanna keep zbuffer contents
//    m_light_pipe.m_effect.m_vs.bind_rotation(Mat3f::identity());
//    m_light_pipe.m_effect.m_vs.bind_translation(Vec3f(m_lpos_x, m_lpos_y, m_lpos_z));
//    m_light_pipe.draw(m_light_itl);
//}
