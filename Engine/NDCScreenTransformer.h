#pragma once
#include "Vec3.h"
#include "Graphics.h"

class NDCScreenTransformer
{
public:
    NDCScreenTransformer();
    template<typename V>
    V &transform(V &v) const;
    template<typename V>
    V get_transformed(const V &v) const;
private:
    float m_xfac;
    float m_yfac;
};

template<typename V>
inline V &NDCScreenTransformer::transform(V &v) const
{
    // perform homogenous -> ndc transformation on xyz
    // perspective-correct interpolative divide on all other attributes
    const float winv = 1.f / v.m_pos.m_w;
    v *= winv;
    // additional divide for maped z because it must be interpolated
    // adjust position x / y from perspective normalized space
    // to screen dimension space after perspective divide
    v.m_pos.m_x = (v.m_pos.m_x + 1.f) * m_xfac;
    v.m_pos.m_y = (v.m_pos.m_y + 1.f) * m_yfac;
    // store 1/w in w (we will need the interpolated 1/w
    // so that we can recover the attributes after interpolation)
    v.m_pos.m_w = winv;

    return v;
}

template<typename V>
inline V NDCScreenTransformer::get_transformed(const V &v) const
{
    return transform(V(v));
}
