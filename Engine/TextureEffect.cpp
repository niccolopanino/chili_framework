#include "TextureEffect.h"

TextureEffect::Vertex TextureEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_tc + rhs.m_tc);
}
TextureEffect::Vertex TextureEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_tc - rhs.m_tc);
}

TextureEffect::Vertex TextureEffect::Vertex::operator*(float rhs) const
{
    return Vertex(m_pos * rhs, m_tc * rhs);
}

TextureEffect::Vertex TextureEffect::Vertex::operator/(float rhs) const
{
    return Vertex(m_pos / rhs, m_tc / rhs);
}

void TextureEffect::PixelShader::bind_texture(const std::wstring &filename)
{
    m_tex = std::make_unique<Surface>(Surface::from_file(filename));
    m_tex_width = float(m_tex->get_width());
    m_tex_height = float(m_tex->get_height());
    m_tex_clamp_x = m_tex_width - 1.f;
    m_tex_clamp_y = m_tex_height - 1.f;
}
