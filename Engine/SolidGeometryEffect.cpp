#include "SolidGeometryEffect.h"

Triangle<SolidGeometryEffect::GeometryShader::Output>
SolidGeometryEffect::GeometryShader::operator()(
    const VertexShader::Output &input0,
    const VertexShader::Output &input1,
    const VertexShader::Output &input2,
    size_t tri_idx) const
{
    return {
        Output(input0.m_pos, m_tri_colors[tri_idx / 2]),
        Output(input1.m_pos, m_tri_colors[tri_idx / 2]),
        Output(input2.m_pos, m_tri_colors[tri_idx / 2])
    };
}
