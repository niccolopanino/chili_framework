#pragma once
#include "Mat.h"

template<typename V>
class BaseVertexShader
{
public:
    typedef V Output;
public:
    void bind_world_view(const Mat4f &world_view);
    void bind_projection(const Mat4f &proj);
    const Mat4f &get_proj() const;
protected:
    Mat4f m_proj = Mat4f::identity();
    Mat4f m_world_view = Mat4f::identity();
    Mat4f m_world_view_proj = Mat4f::identity();
};

template<typename V>
inline void BaseVertexShader<V>::bind_world_view(const Mat4f &world_view)
{
    m_world_view = world_view;
    m_world_view_proj = m_world_view * m_proj;
}

template<typename V>
inline void BaseVertexShader<V>::bind_projection(const Mat4f &proj)
{
    m_proj = proj;
    m_world_view_proj = m_world_view_proj * m_proj;
}

template<typename V>
inline const Mat4f &BaseVertexShader<V>::get_proj() const
{
    return m_proj;
}
