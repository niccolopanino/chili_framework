#include "SolidEffect.h"

void SolidEffect::VertexShader::bind_world(const Mat4f &world)
{
    m_world = world;
    m_world_proj = m_world * m_proj;
}

void SolidEffect::VertexShader::bind_projection(const Mat4f &proj)
{
    m_proj = proj;
    m_world_proj = m_world * m_proj;
}

SolidEffect::VertexShader::Output SolidEffect::VertexShader::operator()(const Vertex &input) const
{
    return Output(Vec4f(input.m_pos) * m_world_proj, input.m_color);
}
