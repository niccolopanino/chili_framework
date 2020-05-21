#pragma once
#include "Mat.h"
#include "Vec3.h"

template<typename V>
class DefaultVertexShader
{
public:
    typedef V Output;
public:
    void bind_rotation(const Mat3f &rot) { m_rot = rot; }
    void  bind_translation(const Vec3f &trans) { m_trans = trans; }
    Output operator()(const V &input) const { return V(input.m_pos * m_rot + m_trans, input); }
private:
    Mat3f m_rot = Mat3f::identity();
    Vec3f m_trans;
};
