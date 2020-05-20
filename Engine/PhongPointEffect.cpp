#include "PhongPointEffect.h"

PhongPointEffect::Vertex PhongPointEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_n + rhs.m_n);
}

PhongPointEffect::Vertex PhongPointEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_n - rhs.m_n);
}

PhongPointEffect::VertexShader::Output::Output(const Vec3f &pos, const Output &src) :
    m_pos(pos), m_n(src.m_n), m_world_pos(src.m_world_pos)
{ }

PhongPointEffect::VertexShader::Output::Output(const Vec3f &pos,
    const Vec3f &n, const Vec3f &world_pos)
    :
    m_pos(pos), m_n(n), m_world_pos(world_pos)
{ }

PhongPointEffect::VertexShader::Output
PhongPointEffect::VertexShader::Output::operator+(const Output &rhs) const
{
    return Output(m_pos + rhs.m_pos, m_n + rhs.m_n, m_world_pos + rhs.m_world_pos);
}

PhongPointEffect::VertexShader::Output
PhongPointEffect::VertexShader::Output::operator-(const Output &rhs) const
{
    return Output(m_pos - rhs.m_pos, m_n - rhs.m_n, m_world_pos - rhs.m_world_pos);
}

PhongPointEffect::VertexShader::Output
PhongPointEffect::VertexShader::Output::operator*(float rhs) const
{
    return Output(m_pos * rhs, m_n * rhs, m_world_pos * rhs);
}

PhongPointEffect::VertexShader::Output
PhongPointEffect::VertexShader::Output::operator/(float rhs) const
{
    return Output(m_pos / rhs, m_n / rhs, m_world_pos / rhs);
}

PhongPointEffect::VertexShader::Output
PhongPointEffect::VertexShader::operator()(const Vertex &input) const
{
    const auto pos = input.m_pos * m_rot + m_trans;
    return Output(pos, input.m_n * m_rot, pos);
}
