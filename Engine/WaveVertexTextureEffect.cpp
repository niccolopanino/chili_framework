#include "WaveVertexTextureEffect.h"

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

void WaveVertexTextureEffect::PixelShader::bind_texture(const std::wstring &filename)
{
    m_tex = std::make_unique<Surface>(Surface::from_file(filename));
    m_tex_width = float(m_tex->get_width());
    m_tex_height = float(m_tex->get_height());
    m_tex_clamp_x = m_tex_width - 1.f;
    m_tex_clamp_y = m_tex_height - 1.f;
}

WaveVertexTextureEffect::VertexShader::Output
WaveVertexTextureEffect::VertexShader::operator()(const Vertex &input) const
{
    Vec3f pos = input.m_pos * m_rot + m_trans;
    pos.m_y += m_amplitude * std::sin(m_time * m_freq_scroll + pos.m_x * m_freq_wave);
    return Output(pos, input);
}
