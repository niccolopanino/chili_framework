#include "VertexFlatEffect.h"
#include <algorithm>
#include <cassert>

VertexFlatEffect::Vertex VertexFlatEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_n + rhs.m_n);
}

VertexFlatEffect::Vertex VertexFlatEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_n - rhs.m_n);
}

VertexFlatEffect::VertexShader::Output
VertexFlatEffect::VertexShader::operator()(const Vertex &input) const
{
    // calculate intensity based on angle of incidence
    const auto d = m_diff * std::max(0.f, Vec3f::dot(-(input.m_n * m_rot), m_dir));
    // add diffuse, ambient, filter by material color, staurate and scale
    const auto c = (m_color * (d + m_amb)).saturate() * 255.f;
    return Output(input.m_pos * m_rot + m_trans, Color(c));
}

void VertexFlatEffect::VertexShader::set_light_direction(const Vec3f &dl)
{
    assert(dl.len_sq() >= .001f);
    m_dir = dl.get_normalized();
}
