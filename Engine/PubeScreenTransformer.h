#pragma once
#include "Vec3.h"
#include "Graphics.h"

class PubeScreenTransformer
{
public:
    PubeScreenTransformer();
    Vec3f &transform(Vec3f &v) const;
    Vec3f get_transformed(const Vec3f &v) const;
private:
    float m_xfac;
    float m_yfac;
};
