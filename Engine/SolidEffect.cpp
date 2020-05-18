#include "SolidEffect.h"

SolidEffect::Vertex SolidEffect::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_color);
}

SolidEffect::Vertex SolidEffect::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_color);
}

SolidEffect::Vertex SolidEffect::Vertex::operator*(float rhs) const
{
    return Vertex(m_pos * rhs, m_color);
}

SolidEffect::Vertex SolidEffect::Vertex::operator/(float rhs) const
{
    return Vertex(m_pos / rhs, m_color);
}
