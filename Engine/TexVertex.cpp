#include "TexVertex.h"

TexVertex TexVertex::interpolate(const TexVertex &dst, float alpha) const
{
    return TexVertex(m_pos.interpolate(dst.m_pos, alpha), m_tc.interpolate(dst.m_tc, alpha));
}
