#include "GouraudEffect.h"
#include <cassert>
#include <algorithm>

GouraudEffect::Vertex GouraudEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_n + rhs.m_n);
}

GouraudEffect::Vertex GouraudEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_n - rhs.m_n);
}

GouraudEffect::VertexShader::Output
GouraudEffect::VertexShader::Output::operator+(const Output &rhs) const
{
    return Output(m_pos + rhs.m_pos, m_color + rhs.m_color);
}

GouraudEffect::VertexShader::Output
GouraudEffect::VertexShader::Output::operator-(const Output &rhs) const
{
    return Output(m_pos - rhs.m_pos, m_color - rhs.m_color);
}

GouraudEffect::VertexShader::Output
GouraudEffect::VertexShader::operator()(const Vertex &input) const
{
    // calculate intensity based on angle of incidence
    const auto d = m_diff * std::max(0.f, Vec3f::dot(-(input.m_n * m_rot), m_dir));
    // add diffuse, ambient, filter by material color, saturate and scale
    const auto c = (m_color * (d + m_amb)).saturate() * 255.f;
    return Output(input.m_pos * m_rot + m_trans, c);
}

void GouraudEffect::VertexShader::set_light_direction(const Vec3f &dl)
{
    assert(dl.len_sq() >= .001f);
    m_dir = dl.get_normalized();
}
