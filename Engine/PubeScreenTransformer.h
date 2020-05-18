#pragma once
#include "Vec3.h"
#include "Graphics.h"

class PubeScreenTransformer
{
public:
    PubeScreenTransformer();
    template<typename V>
    V &transform(V &v) const;
    template<typename V>
    V get_transformed(const V &v) const;
private:
    float m_xfac;
    float m_yfac;
};

template<typename V>
inline V &PubeScreenTransformer::transform(V &v) const
{
    const float zinv = 1.f / v.m_pos.m_z;
    // divide all position components and attributes by z
    // (we want to be interpolating our attributes in the
    // same space where the x / y interpolation is taking
    // place to prevent distortion
    v *= zinv;
    // adjust position x / y from perspective normalized space
    // to screen dimension space after perspective divide
    v.m_pos.m_x = (v.m_pos.m_x + 1.f) * m_xfac;
    v.m_pos.m_y = (v.m_pos.m_y + 1.f) * m_yfac;
    // store 1/z in z (we will need the interpolated 1/z
    // so that we can recover the attributes after interpolation)
    v.m_pos.m_z = zinv;

    return v;
}

template<typename V>
inline V PubeScreenTransformer::get_transformed(const V &v) const
{
    return transform(V(v));
}
