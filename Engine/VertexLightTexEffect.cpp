#include "VertexLightTexEffect.h"
#include "ChiliMath.h"
#include <algorithm>

VertexLightTexEffect::VSOut VertexLightTexEffect::VSOut::operator+(const VSOut &rhs) const
{
    return VSOut(m_pos + rhs.m_pos, m_l + rhs.m_l, m_tc + rhs.m_tc);
}

VertexLightTexEffect::VSOut VertexLightTexEffect::VSOut::operator-(const VSOut &rhs) const
{
    return VSOut(m_pos - rhs.m_pos, m_l - rhs.m_l, m_tc - rhs.m_tc);
}

VertexLightTexEffect::VertexShader::Output
VertexLightTexEffect::VertexShader::operator()(const Vertex &input) const
{
    // transform mech vertex position before lighting calc
    const auto world_pos = input.m_pos * m_world_view;
    // vertex to light data
    const auto v_to_l = m_light_pos - world_pos;
    const auto dist = v_to_l.len();
    const Vec3f dir = v_to_l / dist;
    // calculate attenuation
    const auto att = 1.f / (k_const_att + k_lin_att * dist + k_quad_att * sq(dist));
    // calculate intennsity based on angle of incidence and attenuation
    const auto d = m_light_diff * att * std::max(0.f,
        Vec3f::dot(static_cast<Vec3f>(Vec4f(input.m_n, 0.f) * m_world_view), dir));
    // add diffuse, ambient, filter by material color, saturate and scale
    const auto l = d + m_light_amb;
    return Output(input.m_pos * m_world_view_proj, l, input.m_tc);
}

void VertexLightTexEffect::PixelShader::bind_texture(const Surface &tex)
{
    m_tex = &tex;
    m_tex_width = m_tex->get_width();
    m_tex_height = m_tex->get_height();
}