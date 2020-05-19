#include "WaveVertexTextureEffect.h"
#include <cassert>
#include <algorithm>

WaveVertexTextureEffect::Vertex
WaveVertexTextureEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_tc + rhs.m_tc);
}

WaveVertexTextureEffect::Vertex
WaveVertexTextureEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_tc - rhs.m_tc);
}

WaveVertexTextureEffect::Vertex
WaveVertexTextureEffect::Vertex::operator*(float rhs) const
{
    return Vertex(m_pos * rhs, m_tc * rhs);
}

WaveVertexTextureEffect::Vertex
WaveVertexTextureEffect::Vertex::operator/(float rhs) const
{
    return Vertex(m_pos / rhs, m_tc / rhs);
}

WaveVertexTextureEffect::VertexShader::Output
WaveVertexTextureEffect::VertexShader::operator()(const Vertex &input) const
{
    Vec3f pos = input.m_pos * m_rot + m_trans;
    pos.m_y += m_amplitude * std::sin(m_time * m_freq_scroll + pos.m_x * m_freq_wave);
    return Output(pos, input);
}

WaveVertexTextureEffect::GeometryShader::Output::Output(const Vec3f &pos, const Output &src) :
    m_pos(pos), m_tc(src.m_tc), m_l(src.m_l)
{ }

WaveVertexTextureEffect::GeometryShader::Output
WaveVertexTextureEffect::GeometryShader::Output::operator+(const Output &rhs) const
{
    return Output(m_pos + rhs.m_pos, m_tc + rhs.m_tc, m_l);
}

WaveVertexTextureEffect::GeometryShader::Output
WaveVertexTextureEffect::GeometryShader::Output::operator-(const Output &rhs) const
{
    return Output(m_pos - rhs.m_pos, m_tc - rhs.m_tc, m_l);
}

Triangle<WaveVertexTextureEffect::GeometryShader::Output>
WaveVertexTextureEffect::GeometryShader::operator()(
    const VertexShader::Output &input0,
    const VertexShader::Output &input1,
    const VertexShader::Output &input2,
    size_t tri_idx) const
{
    // calculate face normal
    const auto n = Vec3f::cross(input1.m_pos - input0.m_pos, input2.m_pos - input0.m_pos)
        .get_normalized();
    // calculate intensity based on angle of incidence plus ambient and saturate
    const auto l = std::min(1.f, m_diff * std::max(0.f, Vec3f::dot(-n, m_dir)) + m_amb);
    return {
        { input0.m_pos, input0.m_tc, l },
        { input1.m_pos, input1.m_tc, l },
        { input2.m_pos, input2.m_tc, l }
    };
}

void WaveVertexTextureEffect::GeometryShader::set_light_direction(const Vec3f &dl)
{
    assert(dl.len_sq() >= .001f);
    m_dir = dl.get_normalized();
}

void WaveVertexTextureEffect::PixelShader::bind_texture(const std::wstring &filename)
{
    m_tex = std::make_unique<Surface>(Surface::from_file(filename));
    m_tex_width = float(m_tex->get_width());
    m_tex_height = float(m_tex->get_height());
    m_tex_clamp_x = m_tex_width - 1.f;
    m_tex_clamp_y = m_tex_height - 1.f;
}
