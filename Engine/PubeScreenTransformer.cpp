#include "PubeScreenTransformer.h"

PubeScreenTransformer::PubeScreenTransformer() :
    m_xfac(float(Graphics::k_screen_width) / 2.f),
    m_yfac(float(Graphics::k_screen_height) / 2.f)
{ }

Vec3f &PubeScreenTransformer::transform(Vec3f & v) const
{
    const float zinv = 1.f / v.m_z;
    v.m_x = (v.m_x * zinv + 1.f) * m_xfac;
    v.m_y = (-v.m_y * zinv + 1.f) * m_yfac;
    return v;
}

Vec3f PubeScreenTransformer::get_transformed(const Vec3f &v) const
{
    return transform(Vec3f(v));
}
