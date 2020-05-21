#include "SpecularPointEffect.h"

SpecularPointEffect::Vertex SpecularPointEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_n + rhs.m_n);
}

SpecularPointEffect::Vertex SpecularPointEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_n - rhs.m_n);
}

SpecularPointEffect::VertexShader::Output::Output(const Vec3f &pos, const Output &src) :
    m_pos(pos), m_n(src.m_n), m_world_pos(src.m_world_pos)
{ }

SpecularPointEffect::VertexShader::Output::Output(const Vec3f &pos,
    const Vec3f &n, const Vec3f &world_pos)
    :
    m_pos(pos), m_n(n), m_world_pos(world_pos)
{ }

SpecularPointEffect::VertexShader::Output
SpecularPointEffect::VertexShader::Output::operator+(const Output &rhs) const
{
    return Output(m_pos + rhs.m_pos, m_n + rhs.m_n, m_world_pos + rhs.m_world_pos);
}

SpecularPointEffect::VertexShader::Output
SpecularPointEffect::VertexShader::Output::operator-(const Output &rhs) const
{
    return Output(m_pos - rhs.m_pos, m_n - rhs.m_n, m_world_pos - rhs.m_world_pos);
}

SpecularPointEffect::VertexShader::Output
SpecularPointEffect::VertexShader::Output::operator*(float rhs) const
{
    return Output(m_pos * rhs, m_n * rhs, m_world_pos * rhs);
}

SpecularPointEffect::VertexShader::Output
SpecularPointEffect::VertexShader::Output::operator/(float rhs) const
{
    return Output(m_pos / rhs, m_n / rhs, m_world_pos / rhs);
}

SpecularPointEffect::VertexShader::Output
SpecularPointEffect::VertexShader::operator()(const Vertex &input) const
{
    const auto pos = Vec4f(input.m_pos) * m_transform;
    return Output(pos, Vec4f(input.m_n, 0.f) * m_transform, pos);
}
