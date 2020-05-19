#include "GeometryFlatEffect.h"
#include <algorithm>
#include <cassert>

Triangle<GeometryFlatEffect::GeometryShader::Output>
GeometryFlatEffect::GeometryShader::operator()(
    const VertexShader::Output &input0,
    const VertexShader::Output &input1,
    const VertexShader::Output &input2,
    size_t tri_idx) const
{
    // calculate face normal
    const auto n = Vec3f::cross(input1.m_pos - input0.m_pos, input2.m_pos - input0.m_pos)
        .get_normalized();
    // calculate intensity based on angle of incidence
    const auto d = m_diff * std::max(0.f, Vec3f::dot(-n, m_dir));
    // add diffuse, ambient, filter by material color, saturate and scale
    const auto c = Color((m_color * (d + m_amb)).saturate() * 255.f);
    return { Output(input0.m_pos, c), Output(input1.m_pos, c), Output(input2.m_pos, c) };
}

void GeometryFlatEffect::GeometryShader::set_light_direction(const Vec3f &dl)
{
    assert(dl.len_sq() >= .001f);
    m_dir = dl.get_normalized();
}
