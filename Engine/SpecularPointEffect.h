#pragma once
#include "Vec3.h"
#include "Vec4.h"
#include "Mat.h"
#include "Colors.h"
#include "BaseVertexShader.h"
#include "DefaultGeometryShader.h"
#include "BasePhongShader.h"
#include "ChiliMath.h"
#include <cmath>
#include <algorithm>

template<typename DiffuseParams, typename SpecularParams>
class SpecularPointEffect
{
public:
    // the vertex type that will be input into the pipeline
    class Vertex
    {
    public:
        Vertex() = default;
        Vertex(const Vec3f &pos) : m_pos(pos) { }
        Vertex(const Vec3f &pos, const Vertex &src) : m_pos(pos), m_n(src.m_n) { }
        Vertex(const Vec3f &pos, const Vec3f &n) : m_pos(pos), m_n(n) { }
        Vertex operator+(const Vertex &rhs) const;
        Vertex &operator+=(const Vertex &rhs) { return *this = *this + rhs; }
        Vertex operator-(const Vertex &rhs) const;
        Vertex &operator-=(const Vertex &rhs) { return *this = *this - rhs; }
        Vertex operator*(float rhs) const { return Vertex(m_pos * rhs, m_n * rhs); }
        Vertex &operator*=(float rhs) { return *this = *this * rhs; }
        Vertex operator/(float rhs) const { return Vertex(m_pos / rhs, m_n / rhs); }
        Vertex &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec3f m_pos;
        Vec3f m_n;
    };
    class VSOut
    {
    public:
        VSOut() = default;
        VSOut(const Vec4f &pos) : m_pos(pos) { }
        VSOut(const Vec4f &pos, const VSOut &src);
        VSOut(const Vec4f &pos, const Vec3f &n, const Vec3f &world_pos);
        VSOut operator+(const VSOut &rhs) const;
        VSOut &operator+=(const VSOut &rhs) { return *this = *this + rhs; }
        VSOut operator-(const VSOut &rhs) const;
        VSOut &operator-=(const VSOut &rhs) { return *this = *this - rhs; }
        VSOut operator*(float rhs) const;
        VSOut &operator*=(float rhs) { return *this = *this * rhs; }
        VSOut operator/(float rhs) const;
        VSOut &operator/=(float rhs) { return *this = *this / rhs; }
    public:
        Vec4f m_pos;
        Vec3f m_n;
        Vec3f m_world_pos;
    };
    // duplicate pos to have one "clean" world position
    class VertexShader : public BaseVertexShader<VSOut>
    {
    public:
        typename BaseVertexShader<VSOut>::Output operator()(const Vertex &input) const
        {
            const auto pos = Vec4f(input.m_pos);
            return Output(pos * m_world_view_proj,
                Vec4f(input.m_n, 0.f) * m_world_view, pos * m_world_view);
        }
    };
    // default geometry shader passes vertices through and outputs triangle
    typedef DefaultGeometryShader<typename VertexShader::Output> GeometryShader;
    // invoked for each pixel of a triangle
    // takes an input of attributes that are the
    // result of interpolating vertex attributes
    // and outputs a color
    class PixelShader : public BasePhongShader<DiffuseParams, SpecularParams>
    {
    public:
        template<typename I>
        Color operator()(const I &input) const { return shade(input, m_mat_color); }
    private:
        Vec3f m_mat_color = Vec3f(.8f, .85f, 1.f);
    };
public:
    VertexShader m_vs;
    GeometryShader m_gs;
    PixelShader m_ps;
};

template<typename DiffuseParams, typename SpecularParams>
inline typename SpecularPointEffect<DiffuseParams, SpecularParams>::Vertex
SpecularPointEffect<DiffuseParams, SpecularParams>::Vertex::operator+(const Vertex &rhs) const
{
    return Vertex(m_pos + rhs.m_pos, m_n + rhs.m_n);
}

template<typename DiffuseParams, typename SpecularParams>
inline typename SpecularPointEffect<DiffuseParams, SpecularParams>::Vertex
SpecularPointEffect<DiffuseParams, SpecularParams>::Vertex::operator-(const Vertex &rhs) const
{
    return Vertex(m_pos - rhs.m_pos, m_n - rhs.m_n);
}

template<typename DiffuseParams, typename SpecularParams>
inline SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut::VSOut(const Vec4f &pos,
    const VSOut &src)
    :
    m_pos(pos), m_n(src.m_n), m_world_pos(src.m_world_pos)
{ }

template<typename DiffuseParams, typename SpecularParams>
inline SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut::VSOut(const Vec4f &pos,
    const Vec3f &n, const Vec3f &world_pos)
    :
    m_pos(pos), m_n(n), m_world_pos(world_pos)
{ }

template<typename DiffuseParams, typename SpecularParams>
inline typename SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut
SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut::operator+(const VSOut &rhs) const
{
    return VSOut(m_pos + rhs.m_pos, m_n + rhs.m_n, m_world_pos + rhs.m_world_pos);
}

template<typename DiffuseParams, typename SpecularParams>
inline typename SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut
SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut::operator-(const VSOut &rhs) const
{
    return VSOut(m_pos - rhs.m_pos, m_n - rhs.m_n, m_world_pos - rhs.m_world_pos);
}

template<typename DiffuseParams, typename SpecularParams>
inline typename SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut
SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut::operator*(float rhs) const
{
    return VSOut(m_pos * rhs, m_n * rhs, m_world_pos * rhs);
}

template<typename DiffuseParams, typename SpecularParams>
inline typename SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut
SpecularPointEffect<DiffuseParams, SpecularParams>::VSOut::operator/(float rhs) const
{
    return VSOut(m_pos / rhs, m_n / rhs, m_world_pos / rhs);
}
