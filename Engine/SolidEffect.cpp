#include "SolidEffect.h"

SolidEffect::VertexShader::Output
SolidEffect::VertexShader::operator()(const Vertex &input) const
{
    return Output(Vec4f(input.m_pos) * m_world_view_proj, input.m_color);
}
