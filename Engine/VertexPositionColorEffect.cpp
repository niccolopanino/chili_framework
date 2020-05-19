#include "VertexPositionColorEffect.h"

VertexPositionColorEffect::VertexShader::Output
VertexPositionColorEffect::VertexShader::Output::operator+(const Output &rhs) const
{
    return Output(m_pos + rhs.m_pos, m_color + rhs.m_color);
}

VertexPositionColorEffect::VertexShader::Output
VertexPositionColorEffect::VertexShader::Output::operator-(const Output &rhs) const
{
    return Output(m_pos - rhs.m_pos, m_color - rhs.m_color);
}

VertexPositionColorEffect::VertexShader::Output
VertexPositionColorEffect::VertexShader::operator()(const Vertex &input) const
{
    const auto pos = input.m_pos * m_rot + m_trans;
    return Output(pos, Vec3f(std::abs(pos.m_x), std::abs(pos.m_y), std::abs(pos.m_z)) * 255.f);
}
