#include "TexVertex.h"

TexVertex TexVertex::operator+(const TexVertex &rhs) const
{
    return TexVertex(m_pos + rhs.m_pos, m_tc + rhs.m_tc);
}

TexVertex TexVertex::operator-(const TexVertex &rhs) const
{
    return TexVertex(m_pos - rhs.m_pos, m_tc - rhs.m_tc);
}

TexVertex TexVertex::operator*(float rhs) const
{
    return TexVertex(m_pos * rhs, m_tc * rhs);
}

TexVertex TexVertex::operator/(float rhs) const
{
    return TexVertex(m_pos / rhs, m_tc / rhs);
}

TexVertex TexVertex::interpolate(const TexVertex &dst, float alpha) const
{
    return TexVertex(m_pos.interpolate(dst.m_pos, alpha), m_tc.interpolate(dst.m_tc, alpha));
}
