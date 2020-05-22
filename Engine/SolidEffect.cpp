#include "SolidEffect.h"

void SolidEffect::VertexShader::bind_world_view(const Mat4f &world_view)
{
    m_world_view = world_view;
    m_world_view_proj = m_world_view * m_proj;
}

void SolidEffect::VertexShader::bind_projection(const Mat4f &proj)
{
    m_proj = proj;
    m_world_view_proj = m_world_view * m_proj;
}

SolidEffect::VertexShader::Output SolidEffect::VertexShader::operator()(const Vertex &input) const
{
    return Output(Vec4f(input.m_pos) * m_world_view_proj, input.m_color);
}
