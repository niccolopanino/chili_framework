#include "GouraudPointEffect.h"
#include <cassert>
#include <algorithm>

GouraudPointEffect::Vertex GouraudPointEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_n + rhs.m_n);
}

GouraudPointEffect::Vertex GouraudPointEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_n - rhs.m_n);
}

GouraudPointEffect::VertexShader::Output
GouraudPointEffect::VertexShader::Output::operator+(const Output &rhs) const
{
    return Output(m_pos + rhs.m_pos, m_color + rhs.m_color);
}

GouraudPointEffect::VertexShader::Output
GouraudPointEffect::VertexShader::Output::operator-(const Output &rhs) const
{
    return Output(m_pos - rhs.m_pos, m_color - rhs.m_color);
}

GouraudPointEffect::VertexShader::Output
GouraudPointEffect::VertexShader::operator()(const Vertex &input) const
{
    // transform mech vertex position before lighting calc
    const auto pos = input.m_pos * m_rot + m_trans;
    // vertex to light data
    const auto v_to_l = m_light_pos - pos;
    const auto dist = v_to_l.len();
    const auto dir = v_to_l / dist;
    // calculate attenuation
    const auto attenuation = 1.f
        / (m_const_attenuation + m_lin_attenuation * dist + m_quad_attenuation * sq(dist));
    // calculate intensity based on angle of incidence and attenuation
    const auto d = m_light_diff * attenuation * std::max(0.f, Vec3f::dot(input.m_n * m_rot, dir));
    // add diffuse, ambient, filter by material color, saturate and scale
    const auto c = (m_mat_color * (d + m_light_amb)).saturate() * 255.f;
    return Output(pos, c);
}
