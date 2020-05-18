#include "VertexColorEffect.h"

VertexColorEffect::Vertex VertexColorEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_color + rhs.m_color);
}

VertexColorEffect::Vertex VertexColorEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_color - rhs.m_color);
}

VertexColorEffect::Vertex VertexColorEffect::Vertex::operator*(float rhs) const
{
    return Vertex(m_pos * rhs, m_color * rhs);
}

VertexColorEffect::Vertex VertexColorEffect::Vertex::operator/(float rhs) const
{
    return Vertex(m_pos / rhs, m_color / rhs);
}
